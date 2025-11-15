import { FormEvent, useCallback, useEffect, useMemo, useState } from 'react';
import { COMMAND_TOPIC, EVENT_TOPIC, STATUS_TOPIC, TELEMETRY_TOPIC } from './config';
import { useMqttClient } from './hooks/useMqttClient';
import type {
  ComponentStatus,
  DashboardNotification,
  DatabaseConfig,
  FeedingSession,
  LogEntry,
  MlInferenceResult,
  MlPlugin
} from './types';
import { TelemetryCard } from './components/TelemetryCard';
import { NotificationList } from './components/NotificationList';
import { LogPanel } from './components/LogPanel';
import { DataBridgePanel } from './components/DataBridgePanel';
import { MlPluginPanel } from './components/MlPluginPanel';
import { getPlugins, registerPlugin } from './plugins';

const MAX_SESSIONS = 8;
const MAX_NOTIFICATIONS = 15;

export default function App() {
  const [sessions, setSessions] = useState<FeedingSession[]>([]);
  const [notifications, setNotifications] = useState<DashboardNotification[]>([]);
  const [status, setStatus] = useState<ComponentStatus>({});
  const [chambersInput, setChambersInput] = useState('');
  const [commandFeedback, setCommandFeedback] = useState('');
  const [logs, setLogs] = useState<LogEntry[]>([]);
  const [dbConfig, setDbConfig] = useState<DatabaseConfig>({
    endpoint: '',
    apiKey: '',
    camelTable: 'camel_profiles',
    amountField: 'dailyPortion'
  });
  const [dbStatus, setDbStatus] = useState('');
  const [syncingDb, setSyncingDb] = useState(false);
  const [mlResults, setMlResults] = useState<MlInferenceResult[]>([]);
  const [mlStatus, setMlStatus] = useState('');
  const [mlRunning, setMlRunning] = useState(false);
  const [pluginVersion, setPluginVersion] = useState(0);

  const handleMessage = useCallback((topic: string, payload: string) => {
    try {
      const data = JSON.parse(payload);
      if (topic === TELEMETRY_TOPIC) {
        setSessions((prev) => [data as FeedingSession, ...prev].slice(0, MAX_SESSIONS));
      } else if (topic === EVENT_TOPIC) {
        const event: DashboardNotification = {
          event: data.event || 'event',
          detail: data.detail,
          timestamp: data.timestamp || Date.now()
        };
        setNotifications((prev) => [event, ...prev].slice(0, MAX_NOTIFICATIONS));
      } else if (topic === STATUS_TOPIC) {
        setStatus(data as ComponentStatus);
      }
    } catch (error) {
      console.warn('Unable to parse MQTT payload', error);
    }
  }, []);

  const appendLog = useCallback((level: LogEntry['level'], message: string) => {
    const entry: LogEntry = { level, message, timestamp: Date.now() };
    setLogs((prev) => [entry, ...prev].slice(0, 200));
  }, []);

  const { status: connectionStatus, publish } = useMqttClient({
    topics: [TELEMETRY_TOPIC, EVENT_TOPIC, STATUS_TOPIC],
    onMessage: handleMessage,
    onLog: appendLog
  });

  useEffect(() => {
    const globalScope = window as typeof window & { registerCamelMlPlugin?: (plugin: MlPlugin) => void };
    globalScope.registerCamelMlPlugin = (plugin: MlPlugin) => {
      registerPlugin(plugin);
      setPluginVersion((prev) => prev + 1);
      appendLog('info', `ML plugin registered: ${plugin.name}`);
    };
    return () => {
      delete globalScope.registerCamelMlPlugin;
    };
  }, [appendLog]);

  const plugins = useMemo(() => getPlugins(), [pluginVersion]);

  const latestSession = sessions[0];
  const statusItems = useMemo(
    () => [
      { key: 'motion', label: 'Motion Sensor' },
      { key: 'rfid', label: 'RFID' },
      { key: 'scale', label: 'Scale' },
      { key: 'network', label: 'Network' },
      { key: 'time', label: 'Clock' },
      { key: 'motor', label: 'Motor' }
    ],
    []
  );

  const handleCommand = (event: FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    const chambers = Number(chambersInput);
    if (!Number.isFinite(chambers) || chambers <= 0) {
      setCommandFeedback('Provide a positive chamber count.');
      return;
    }

    const sent = publish(COMMAND_TOPIC, {
      command: 'refill',
      chambers
    });

    setCommandFeedback(sent ? 'Refill command sent.' : 'Unable to send command. Check MQTT connection.');
    if (sent) {
      setChambersInput('');
      appendLog('info', `Refill command published with ${chambers} chambers.`);
    }
  };

  const handleDbConfigChange = (patch: Partial<DatabaseConfig>) => {
    setDbConfig((prev) => ({ ...prev, ...patch }));
  };

  const syncDatabase = async () => {
    if (!dbConfig.endpoint) {
      setDbStatus('Provide an endpoint URL.');
      return;
    }

    setSyncingDb(true);
    setDbStatus('Fetching data…');
    try {
      const response = await fetch(dbConfig.endpoint, {
        headers: {
          'Content-Type': 'application/json',
          ...(dbConfig.apiKey ? { Authorization: `Bearer ${dbConfig.apiKey}` } : {})
        }
      });
      if (!response.ok) {
        throw new Error(`HTTP ${response.status}`);
      }
      const payload = await response.json();
      const records = Array.isArray(payload) ? payload : payload.data ?? [];
      if (!Array.isArray(records) || records.length === 0) {
        setDbStatus('No records returned from the endpoint.');
        return;
      }

      let published = 0;
      records.forEach((record) => {
        const camelUID = record.camelUID || record.camel_id || record.id;
        const amount = record[dbConfig.amountField] ?? record.amountDropped ?? record.amount;
        if (!camelUID || !amount) {
          return;
        }

        const sent = publish(COMMAND_TOPIC, {
          command: 'set_feeding_profile',
          camelUID,
          amount,
          source: dbConfig.camelTable
        });
        if (sent) {
          published += 1;
        } else {
          appendLog('warn', `MQTT publish skipped (offline) for camel ${camelUID}.`);
        }
      });
      setDbStatus(`Synced ${records.length} records, published ${published} profiles.`);
      appendLog('info', `Database sync published ${published} feeding profiles.`);
    } catch (error) {
      const message = error instanceof Error ? error.message : 'Unknown error';
      setDbStatus(`Sync failed: ${message}`);
      appendLog('error', `Database sync failed: ${message}`);
    } finally {
      setSyncingDb(false);
    }
  };

  const runMlPlugins = async () => {
    if (sessions.length === 0) {
      setMlStatus('Telemetry required before running inference.');
      return;
    }
    if (plugins.length === 0) {
      setMlStatus('No plugins registered.');
      return;
    }
    setMlRunning(true);
    setMlStatus('Running inference…');
    try {
      const outputs: MlInferenceResult[] = [];
      for (const plugin of plugins) {
        appendLog('info', `Running plugin ${plugin.name} (${plugin.id})`);
        const result = await plugin.infer(sessions);
        outputs.push(...result);
      }
      setMlResults(outputs);
      setMlStatus(`Completed inference. ${outputs.length} candidate traits generated.`);
    } catch (error) {
      const message = error instanceof Error ? error.message : 'Unknown error';
      setMlStatus(`Inference failed: ${message}`);
      appendLog('error', `ML inference failed: ${message}`);
    } finally {
      setMlRunning(false);
    }
  };

  const clearLogs = () => setLogs([]);

  return (
    <div className="app">
      <header className="page-header">
        <div>
          <p className="muted small">Camel Feeding Station</p>
          <h1>Operations Dashboard</h1>
        </div>
        <div className={`status-indicator status-${connectionStatus}`}>
          <span className="dot" />
          <span>{connectionStatus}</span>
        </div>
      </header>

      <main className="grid">
        <TelemetryCard session={latestSession} />

        <section className="card">
          <h2>Component Health</h2>
          <div className="status-grid">
            {statusItems.map((item) => {
              const value = status[item.key as keyof ComponentStatus];
              return (
                <article key={item.key} className={`status-tile ${value ? 'ok' : 'warn'}`}>
                  <p className="label">{item.label}</p>
                  <p className="value">{value ? 'OK' : 'Check'}</p>
                </article>
              );
            })}
            <article className={`status-tile ${status.ready ? 'ok' : 'warn'}`}>
              <p className="label">Dispenser</p>
              <p className="value">{status.ready ? 'Ready' : 'Paused'}</p>
            </article>
            <article className="status-tile neutral">
              <p className="label">Chambers</p>
              <p className="value">{status.chambers ?? '—'}</p>
            </article>
          </div>
        </section>

        <section className="card">
          <h2>Recent Sessions</h2>
          {sessions.length === 0 ? (
            <p className="muted">No telemetry yet.</p>
          ) : (
            <div className="table-wrapper">
              <table>
                <thead>
                  <tr>
                    <th>Camel</th>
                    <th>Food</th>
                    <th>Dropped (g)</th>
                    <th>Consumed (g)</th>
                    <th>Time (s)</th>
                    <th>When</th>
                  </tr>
                </thead>
                <tbody>
                  {sessions.map((session) => (
                    <tr key={`${session.camelUID}-${session.time}`}>
                      <td>{session.camelUID}</td>
                      <td>{session.foodType}</td>
                      <td>{session.amountDropped}</td>
                      <td>{session.weightOfConsumedFood}</td>
                      <td>{session.consumptionTime}</td>
                      <td>{session.time}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}
        </section>

        <NotificationList notifications={notifications} />

        <section className="card">
          <h2>Commands</h2>
          <form className="command-form" onSubmit={handleCommand}>
            <label>
              Chambers Available
              <input
                type="number"
                min={1}
                placeholder="e.g. 9"
                value={chambersInput}
                onChange={(e) => setChambersInput(e.target.value)}
              />
            </label>
            <button type="submit" disabled={connectionStatus !== 'connected'}>
              Send Refill Command
            </button>
          </form>
          {commandFeedback && <p className="muted small">{commandFeedback}</p>}
        </section>

        <DataBridgePanel
          config={dbConfig}
          status={dbStatus}
          syncing={syncingDb}
          onChange={handleDbConfigChange}
          onSync={syncDatabase}
        />

        <MlPluginPanel plugins={plugins} results={mlResults} status={mlStatus} running={mlRunning} onRun={runMlPlugins} />

        <LogPanel logs={logs} onClear={clearLogs} />
      </main>
    </div>
  );
}

import type { DatabaseConfig } from '../types';

interface Props {
  config: DatabaseConfig;
  status: string;
  syncing: boolean;
  onChange: (patch: Partial<DatabaseConfig>) => void;
  onSync: () => void;
}

export function DataBridgePanel({ config, status, syncing, onChange, onSync }: Props) {
  return (
    <section className="card">
      <h2>Database Bridge</h2>
      <p className="muted small">
        Point the dashboard at your database or REST API. Retrieved camel profiles are converted into MQTT commands so the ESP8266
        can adjust feed/ID rules in real-time.
      </p>
      <div className="form-grid">
        <label>
          Endpoint URL
          <input
            type="url"
            placeholder="https://api.example.com/camels"
            value={config.endpoint}
            onChange={(e) => onChange({ endpoint: e.target.value })}
          />
        </label>
        <label>
          API Key / Token
          <input
            type="text"
            placeholder="optional"
            value={config.apiKey}
            onChange={(e) => onChange({ apiKey: e.target.value })}
          />
        </label>
        <label>
          Camel Table / Collection Name
          <input
            type="text"
            placeholder="camel_profiles"
            value={config.camelTable}
            onChange={(e) => onChange({ camelTable: e.target.value })}
          />
        </label>
        <label>
          Amount Field
          <input
            type="text"
            placeholder="dailyPortion"
            value={config.amountField}
            onChange={(e) => onChange({ amountField: e.target.value })}
          />
        </label>
      </div>
      <button type="button" onClick={onSync} disabled={syncing}>
        {syncing ? 'Syncing…' : 'Sync & Push Profiles'}
      </button>
      {status && <p className="muted small">{status}</p>}
    </section>
  );
}

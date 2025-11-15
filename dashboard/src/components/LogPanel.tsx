import type { LogEntry } from '../types';

interface Props {
  logs: LogEntry[];
  onClear: () => void;
}

const levelColor: Record<LogEntry['level'], string> = {
  info: '#2563eb',
  warn: '#d97706',
  error: '#dc2626'
};

export function LogPanel({ logs, onClear }: Props) {
  return (
    <section className="card">
      <header className="panel-header">
        <h2>Activity Log</h2>
        <button type="button" onClick={onClear} disabled={logs.length === 0}>
          Clear
        </button>
      </header>
      {logs.length === 0 ? (
        <p className="muted">No log entries yet.</p>
      ) : (
        <ul className="log-list">
          {logs.map((log) => (
            <li key={`${log.timestamp}-${log.message}`}>
              <span className="log-level" style={{ color: levelColor[log.level] }}>
                {log.level}
              </span>
              <span className="log-message">{log.message}</span>
              <span className="muted small">{new Date(log.timestamp).toLocaleTimeString()}</span>
            </li>
          ))}
        </ul>
      )}
    </section>
  );
}

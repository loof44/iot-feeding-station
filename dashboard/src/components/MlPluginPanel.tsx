import type { MlInferenceResult, MlPlugin } from '../types';

interface Props {
  plugins: MlPlugin[];
  results: MlInferenceResult[];
  status: string;
  running: boolean;
  onRun: () => void;
}

export function MlPluginPanel({ plugins, results, status, running, onRun }: Props) {
  return (
    <section className="card">
      <header className="panel-header">
        <div>
          <h2>ML Plugins</h2>
          <p className="muted small">
            {plugins.length === 0
              ? 'No plugins registered. Use registerCamelMlPlugin(plugin) in the browser console to add one.'
              : `${plugins.length} plugin${plugins.length > 1 ? 's' : ''} available.`}
          </p>
        </div>
        <button type="button" onClick={onRun} disabled={running || plugins.length === 0}>
          {running ? 'Running…' : 'Run Inference'}
        </button>
      </header>
      {status && <p className="muted small">{status}</p>}
      {results.length > 0 && (
        <div className="table-wrapper">
          <table>
            <thead>
              <tr>
                <th>Camel</th>
                <th>Score</th>
                <th>Traits</th>
                <th>Plugin</th>
                <th>Notes</th>
              </tr>
            </thead>
            <tbody>
              {results.map((result, index) => (
                <tr key={`${result.camelUID}-${result.pluginId}-${index}`}>
                  <td>{result.camelUID}</td>
                  <td>{(Math.round(result.score * 100) / 100).toFixed(2)}</td>
                  <td>{result.traits.join(', ')}</td>
                  <td>{result.pluginId}</td>
                  <td>{result.notes ?? '—'}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </section>
  );
}

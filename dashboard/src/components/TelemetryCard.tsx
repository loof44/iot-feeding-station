import type { FeedingSession } from '../types';

interface Props {
  session?: FeedingSession;
}

const formatNumber = (value?: number, unit?: string) => {
  if (value === undefined || value === null || Number.isNaN(value)) {
    return '—';
  }
  const rounded = Math.round(value * 10) / 10;
  return unit ? `${rounded} ${unit}` : String(rounded);
};

export function TelemetryCard({ session }: Props) {
  if (!session) {
    return (
      <section className="card">
        <h2>Latest Feeding Session</h2>
        <p className="muted">Waiting for telemetry…</p>
      </section>
    );
  }

  return (
    <section className="card">
      <h2>Latest Feeding Session</h2>
      <div className="session-grid">
        <div>
          <p className="label">Camel ID</p>
          <p className="value">{session.camelUID}</p>
        </div>
        <div>
          <p className="label">Timestamp</p>
          <p className="value">{session.time}</p>
        </div>
        <div>
          <p className="label">Food Type</p>
          <p className="value">{session.foodType}</p>
        </div>
        <div>
          <p className="label">Amount Dropped</p>
          <p className="value">{formatNumber(session.amountDropped, 'g')}</p>
        </div>
        <div>
          <p className="label">Consumed</p>
          <p className="value">{formatNumber(session.weightOfConsumedFood, 'g')}</p>
        </div>
        <div>
          <p className="label">Consumption Time</p>
          <p className="value">{formatNumber(session.consumptionTime, 's')}</p>
        </div>
        <div>
          <p className="label">Chambers Remaining</p>
          <p className="value">{session.chambersRemaining ?? '—'}</p>
        </div>
      </div>
    </section>
  );
}

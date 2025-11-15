import type { DashboardNotification } from '../types';

interface Props {
  notifications: DashboardNotification[];
}

export function NotificationList({ notifications }: Props) {
  return (
    <section className="card">
      <h2>Notifications</h2>
      {notifications.length === 0 ? (
        <p className="muted">No events yet.</p>
      ) : (
        <ul className="notifications">
          {notifications.map((item) => (
            <li key={`${item.event}-${item.timestamp}`}>
              <div>
                <p className="value">{item.event}</p>
                {item.detail && <p className="muted small">{item.detail}</p>}
              </div>
              <span className="muted small">{new Date(item.timestamp).toLocaleTimeString()}</span>
            </li>
          ))}
        </ul>
      )}
    </section>
  );
}

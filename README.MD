
## Firmware Overview

The `src` folder contains the PlatformIO firmware for the NodeMCU v2 controller that runs the smart feeding station. The sketch now:

- Connects directly to Wi-Fi and publishes telemetry to an MQTT broker.
- Streams structured JSON for telemetry (`feeding/data`), component health (`feeding/status/components`), and high-level events (`feeding/events`).
- Accepts MQTT commands on `feeding/commands` (e.g., refill requests) so that the dashboard or any backend can control the feeder.

Before uploading via PlatformIO, update the Wi-Fi credentials and MQTT settings at the top of `src/main.cpp` if needed.

## Dashboard

A React/Vite dashboard lives in the `dashboard/` folder. It connects to the same MQTT broker over WebSockets and offers:

- Live telemetry & historical session table.
- Component health visualization.
- Event/notification timeline.
- Simple command form to trigger a refill (publishes to `feeding/commands`).

### Running the dashboard

```bash
cd dashboard
npm install
cp .env.example .env   # edit MQTT URL, username, password if they differ
npm run dev            # http://localhost:5173
```

For production deployments run `npm run build` and host the generated `dist/` folder.

### Database + ML extensions

- **Database bridge:** inside the dashboard, open the “Database Bridge” card, point it at your REST/DB API (URL + optional token) and press “Sync & Push Profiles”. Each record should expose `camelUID` (or `camel_id`) plus the feed amount field; the dashboard converts them into MQTT control messages for the ESP firmware (`command: set_feeding_profile`).
- **ML plugins:** the “ML Plugins” panel runs any scoring logic that implements the `MlPlugin` interface (`infer(sessions) -> traits`). Ship plugins by editing `dashboard/src/plugins.ts` or injecting at runtime via the browser console:
  ```js
  registerCamelMlPlugin({
    id: 'my-model',
    name: 'TensorFlow Racing Model',
    version: '0.1.0',
    async infer(sessions) {
      // call your hosted model and return [{ camelUID, score, traits, notes }]
    }
  });
  ```
- **Logging:** the Activity Log card mirrors MQTT connection status, DB sync attempts, and plugin execution so field teams can audit what happened without opening the browser devtools.

## License

See LICENSE for more details.

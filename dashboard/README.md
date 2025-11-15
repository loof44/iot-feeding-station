# Camel Feeding Dashboard

A lightweight React + Vite dashboard that subscribes to the MQTT topics emitted by the NodeMCU firmware. It visualizes the latest telemetry, component health, notifications, and allows sending simple commands (e.g., refill chambers).

## Getting Started

1. Install dependencies:

```bash
cd dashboard
npm install
```

2. Configure environment variables:

```bash
cp .env.example .env
# edit .env to point to your MQTT broker & topics
```

3. Start the dev server:

```bash
npm run dev
```

The dashboard will be available on <http://localhost:5173>.

## Production Build

```bash
npm run build
npm run preview # optional local preview of the static build
```

Deploy the `dashboard/dist` folder to any static hosting provider.

## Activity Logging

All MQTT lifecycle events, command publications, and integration actions are written to the in-app **Activity Log** panel (and the browser console). Use it to confirm subscriptions, diagnose disconnects, and verify when external systems pushed updates.

## Database Bridge

Open the **Database Bridge** card in the UI and set:

- `Endpoint URL` – a REST endpoint that returns camel profiles (array or `{ data: [...] }`).
- `API Key / Token` – optional bearer token header.
- `Amount Field` – the property name that holds the amount to dispense (defaults to `dailyPortion`).

Click **Sync & Push Profiles** to fetch the dataset and publish `set_feeding_profile` MQTT commands toward the ESP firmware. Each record should include `camelUID` (or `camel_id`) and the configured amount field.

## ML Plugins

The **ML Plugins** panel runs any number of pluggable inference modules. Plugins follow:

```ts
import type { MlPlugin } from './types';

const plugin: MlPlugin = {
  id: 'my-model',
  name: 'Racing Predictor',
  version: '1.0.0',
  async infer(sessions) {
    // sessions = recent telemetry entries
    return [{ camelUID: 'A1', score: 0.92, traits: ['Speedy'], notes: '...'}];
  }
};
```

Ship plugins by editing `src/plugins.ts` or register them live in the browser console:

```js
registerCamelMlPlugin(plugin);
```

Running inference aggregates scores/traits so trainers can flag promising racing camels.

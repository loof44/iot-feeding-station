const env = import.meta.env;

export const MQTT_URL = env.VITE_MQTT_URL || '';
export const MQTT_USERNAME = env.VITE_MQTT_USERNAME || '';
export const MQTT_PASSWORD = env.VITE_MQTT_PASSWORD || '';
export const TELEMETRY_TOPIC = env.VITE_TELEMETRY_TOPIC || 'feeding/data';
export const EVENT_TOPIC = env.VITE_EVENT_TOPIC || 'feeding/events';
export const STATUS_TOPIC = env.VITE_STATUS_TOPIC || 'feeding/status/components';
export const COMMAND_TOPIC = env.VITE_COMMAND_TOPIC || 'feeding/commands';

export interface FeedingSession {
  camelUID: string;
  time: string;
  weightOfConsumedFood: number;
  consumptionTime: number;
  foodType: string;
  amountDropped: number;
  chambersRemaining?: number;
}

export interface ComponentStatus {
  motion?: boolean;
  rfid?: boolean;
  scale?: boolean;
  network?: boolean;
  time?: boolean;
  motor?: boolean;
  ready?: boolean;
  chambers?: number;
}

export interface DashboardNotification {
  event: string;
  detail?: string;
  timestamp: number;
}

export type LogLevel = 'info' | 'warn' | 'error';

export interface LogEntry {
  level: LogLevel;
  message: string;
  timestamp: number;
}

export interface DatabaseConfig {
  endpoint: string;
  apiKey: string;
  camelTable: string;
  amountField: string;
}

export interface UseMqttOptions {
  topics: string[];
  onMessage: (topic: string, payload: string) => void;
  onLog?: (level: LogLevel, message: string) => void;
}

export interface MlInferenceResult {
  camelUID: string;
  score: number;
  traits: string[];
  notes?: string;
  pluginId: string;
}

export interface MlPlugin {
  id: string;
  name: string;
  version: string;
  description?: string;
  infer: (sessions: FeedingSession[]) => Promise<MlInferenceResult[]>;
}

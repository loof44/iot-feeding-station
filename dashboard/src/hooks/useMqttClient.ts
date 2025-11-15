import { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import mqtt, { MqttClient } from 'mqtt';
import { MQTT_PASSWORD, MQTT_URL, MQTT_USERNAME } from '../config';
import type { LogLevel, UseMqttOptions } from '../types';

const topicsKey = (topics: string[]) => topics.slice().sort().join(',');

export function useMqttClient({ topics, onMessage, onLog }: UseMqttOptions) {
  const [status, setStatus] = useState<'idle' | 'connecting' | 'connected' | 'error'>(MQTT_URL ? 'connecting' : 'error');
  const clientRef = useRef<MqttClient | null>(null);
  const memoizedTopics = useMemo(() => topicsKey(topics), [topics]);
  const topicList = useMemo(() => topics.slice(), [memoizedTopics]);
  const emitLog = useCallback(
    (level: LogLevel, message: string) => {
      onLog?.(level, message);
    },
    [onLog]
  );

  useEffect(() => {
    if (!MQTT_URL) {
      setStatus('error');
      emitLog('error', 'Missing MQTT_URL. Update dashboard/.env to enable MQTT logging.');
      return;
    }

    setStatus('connecting');
    const client = mqtt.connect(MQTT_URL, {
      username: MQTT_USERNAME || undefined,
      password: MQTT_PASSWORD || undefined,
      reconnectPeriod: 3000,
      connectTimeout: 5000,
      clean: true,
      protocolVersion: 5
    });

    clientRef.current = client;

    const handleConnect = () => {
      setStatus('connected');
      emitLog('info', 'Connected to MQTT broker.');
      topicList.forEach((topic) => {
        client.subscribe(topic);
        emitLog('info', `Subscribed to ${topic}`);
      });
    };

    const handleReconnect = () => {
      setStatus('connecting');
      emitLog('warn', 'Attempting to reconnect to MQTT broker.');
    };
    const handleClose = () => {
      setStatus('idle');
      emitLog('warn', 'MQTT connection closed.');
    };
    const handleError = (err: Error) => {
      setStatus('error');
      emitLog('error', `MQTT error: ${err.message}`);
    };
    const handleMessage = (topic: string, payload: Uint8Array) => {
      const text = new TextDecoder().decode(payload);
      emitLog('info', `MQTT message on ${topic}: ${text.slice(0, 120)}`);
      onMessage(topic, text);
    };

    client.on('connect', handleConnect);
    client.on('reconnect', handleReconnect);
    client.on('close', handleClose);
    client.on('error', handleError);
    client.on('message', handleMessage);

    return () => {
      client.removeListener('connect', handleConnect);
      client.removeListener('reconnect', handleReconnect);
      client.removeListener('close', handleClose);
      client.removeListener('error', handleError);
      client.removeListener('message', handleMessage);
      client.end(true);
      clientRef.current = null;
    };
  }, [emitLog, memoizedTopics, onMessage, topicList]);

  const publish = useCallback((topic: string, payload: Record<string, unknown>) => {
    if (!clientRef.current || status !== 'connected') {
      return false;
    }
    const body = JSON.stringify(payload);
    clientRef.current.publish(topic, body);
    return true;
  }, [status]);

  return { status, publish };
}

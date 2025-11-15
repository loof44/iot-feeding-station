import type { FeedingSession, MlInferenceResult, MlPlugin } from './types';

const staticPlugins: MlPlugin[] = [
  {
    id: 'baseline-consumption-v1',
    name: 'Baseline Consumption Heuristic',
    version: '1.0.0',
    description: 'Ranks camels that finish fast while consuming >80% of the drop. Useful as a placeholder until a real ML model is attached.',
    async infer(sessions: FeedingSession[]): Promise<MlInferenceResult[]> {
      const latestByCamel = new Map<string, FeedingSession[]>();
      sessions.forEach((session) => {
        const list = latestByCamel.get(session.camelUID) ?? [];
        list.push(session);
        latestByCamel.set(session.camelUID, list);
      });

      const results: MlInferenceResult[] = [];
      latestByCamel.forEach((list, camelUID) => {
        const recent = list.slice(0, 5);
        const avgTime = recent.reduce((sum, item) => sum + item.consumptionTime, 0) / recent.length;
        const avgConsumption = recent.reduce((sum, item) => sum + item.weightOfConsumedFood, 0) / recent.length;
        const avgDrop = recent.reduce((sum, item) => sum + item.amountDropped, 0) / recent.length;
        const completionRatio = avgDrop > 0 ? avgConsumption / avgDrop : 0;
        const score = completionRatio * 0.7 + (avgTime > 0 ? (150 - Math.min(avgTime, 150)) / 150 * 0.3 : 0);

        results.push({
          camelUID,
          score,
          traits: [completionRatio > 0.85 ? 'High appetite' : 'Picky', avgTime < 90 ? 'Fast eater' : 'Steady'],
          notes: `Avg time ${avgTime.toFixed(1)}s, consumption ${(completionRatio * 100).toFixed(0)}%`,
          pluginId: 'baseline-consumption-v1'
        });
      });
      return results.sort((a, b) => b.score - a.score).slice(0, 10);
    }
  }
];

const dynamicPlugins: MlPlugin[] = [];

export function registerPlugin(plugin: MlPlugin) {
  dynamicPlugins.push(plugin);
}

export function getPlugins() {
  return [...staticPlugins, ...dynamicPlugins];
}

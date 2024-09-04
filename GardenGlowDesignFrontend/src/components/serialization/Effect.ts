export class Effect {
  name: string;
  startTime: number;
  duration: number;
  layer: number;

  constructor(name: string, startTimeMs: number, duration: number, layer: number) {
    this.name = name;
    this.startTime = startTimeMs;
    this.duration = duration;
    this.layer = layer;
  }

  toJSON() {
    return {
      name: this.name,
      startTime: this.startTime,
      duration: this.duration,
      layer: this.layer
    };
  }
}

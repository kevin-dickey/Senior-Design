export class Effect {
    name: string;
    startTimeMs: number;
    duration: number;
    layer: number;

    constructor(name: string, startTimeMs: number, duration: number, layer: number) {
        this.name = name;
        this.startTimeMs = startTimeMs;
        this.duration = duration;
        this.layer = layer;
    }

    static emptyEffect() {
        return new Effect('', -1, -1, -1);
    }

    toJSON() {
        return {
            name: this.name,
            startTime: this.startTimeMs,
            duration: this.duration,
            layer: this.layer
        };
    }
}

export class RainbowEffect extends Effect {
    colors: string[];
    speed: number;

    constructor(startTimeMs: number,
                duration: number,
                layer: number,
                colors: string[],
                speed: number,
                name: string = 'rainbow') {
        super(name, startTimeMs, duration, layer);
        this.colors = colors;
        this.speed = speed;
    }

    static emptyEffect() {
        return new RainbowEffect(-1, -1, -1, [], -1);
    }
}
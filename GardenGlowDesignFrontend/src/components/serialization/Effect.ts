import {Coordinate} from "./Coordinate";

export class Translation {
    end_pos: Coordinate;
    durationMs: number;

    constructor(end_pos: Coordinate, durationMs: number) {
        this.end_pos = end_pos;
        this.durationMs = durationMs;
    }
}

export class Effect {
    name: string;
    origin: Coordinate;
    startTimeMs: number;
    durationMs: number;
    translation?: Translation;

    constructor(name: string,
                origin: Coordinate,
                startTimeMs: number,
                durationMs: number) {
        this.name = name;
        this.origin = origin;
        this.startTimeMs = startTimeMs;
        this.durationMs = durationMs;
    }

    static emptyEffect() {
        return new Effect('', new Coordinate(0, 0), -1, -1);
    }

    toJSON() {
        return {
            name: this.name,
            startTime: this.startTimeMs,
            duration: this.durationMs,
        };
    }
}

export class RainbowEffect extends Effect {
    colors: string[];
    speed: number;

    constructor(startTimeMs: number,
                durationMs: number,
                colors: string[],
                speed: number,
                name: string = 'rainbow') {
        super(name, new Coordinate(0, 0), startTimeMs, durationMs);
        this.colors = colors;
        this.speed = speed;
    }

    static emptyEffect() {
        return new RainbowEffect(-1, -1, [], -1);
    }
}
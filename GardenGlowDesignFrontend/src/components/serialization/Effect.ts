import {Pair} from "./Pair";

export class Translation {
    end_pos: Pair;
    durationMs: number;

    constructor(end_pos: Pair, durationMs: number) {
        this.end_pos = end_pos;
        this.durationMs = durationMs;
    }
}

export class Effect {
    id: number;
    name: string;
    origin: Pair;
    size: Pair;
    startTimeMs: number;
    durationMs: number;
    translation?: Translation;

    constructor(name: string,
                origin: Pair,
                size: Pair,
                startTimeMs: number,
                durationMs: number,
                translation?: Translation,
                id: number = -1) {
        this.id = id;
        this.name = name;
        this.origin = origin;
        this.size = size;
        this.startTimeMs = startTimeMs;
        this.durationMs = durationMs;
        this.translation = translation
    }

    static emptyEffect() {
        return new Effect('', new Pair(0, 0), new Pair(16, 16), -1, -1);
    }

    toJSON() {
        return {
            name: this.name,
            startTime: this.startTimeMs,
            duration: this.durationMs,
            origin: this.origin,
            size: this.size,
            translation: this.translation
        };
    }
}

export class RainbowEffect extends Effect {
    colors: string[];
    speed: number;

    constructor(
        origin: Pair,
        size: Pair,
        startTimeMs: number,
        durationMs: number,
        colors: string[],
        speed: number,
        name: string = 'rainbow',
        id: number = -1
    ) {
        super(name, origin, size, startTimeMs, durationMs, undefined, id);
        this.colors = colors;
        this.speed = speed;
    }

    static emptyEffect() {
        return new RainbowEffect(new Pair(0, 0), new Pair(20, 20), -1, -1, [], -1);
    }

    toJSON() {
        // Add the colors and speed to the JSON object
        return {
            ...super.toJSON(),
            colors: this.colors,
            speed: this.speed,
        };
    }
}

export class RippleEffect extends Effect {
    ripple_origin: Pair

    constructor(
        origin: Pair,
        size: Pair,
        startTimeMs: number,
        durationMs: number,
        ripple_origin: Pair,
        translation?: Translation,
        name: string = 'ripple',
        id: number = -1
    ) {
        super(name, origin, size, startTimeMs, durationMs, translation, id);
        this.ripple_origin = ripple_origin;
    }

    emptyEffect() {
        return new RippleEffect(new Pair(0, 0), new Pair(20, 20), -1, -1, new Pair(0, 0));
    }

    toJSON() {
        return {
            ...super.toJSON(),
            ripple_origin: this.ripple_origin,
        };
    }
}
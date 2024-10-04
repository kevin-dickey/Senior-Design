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
    type: string;
    name: string;
    layer: number;
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
                id: number = -1,
                type: string = "Effect") {
        this.id = id;
        this.name = name;
        this.type = type;
        // FIXME: Add support for multiple layers
        this.layer = 1;
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
            id: this.id,
            type: this.name,
            name: this.name,
            startTime: this.startTimeMs,
            duration: this.durationMs,
            origin: this.origin,
            size: this.size,
            translation: this.translation
        };
    }

    static fromJSON(data: any) {
        const effect = new Effect(
            data.name,
            data.origin,
            data.size,
            data.startTime,
            data.duration,
            data.translation,
            data.id
        );
        return effect;
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
        super(name, origin, size, startTimeMs, durationMs, undefined, id, 'RainbowEffect');
        this.colors = colors;
        this.speed = speed;
    }

    static fromEffect(effect: Effect, colors: string[], speed: number): RainbowEffect {
        return new RainbowEffect(
            effect.origin,
            effect.size,
            effect.startTimeMs,
            effect.durationMs,
            colors,
            speed,
            effect.name,
            effect.id);
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

    static fromJSON(data: any) {
        return RainbowEffect.fromEffect(
            super.fromJSON(data),
            data.colors,
            data.speed
        );
    }
}

export class RippleEffect extends Effect {
    ripple_origin: Pair
    speed: number;

    constructor(
        origin: Pair,
        size: Pair,
        startTimeMs: number,
        durationMs: number,
        ripple_origin: Pair,
        speed: number,
        translation?: Translation,
        name: string = 'ripple',
        id: number = -1
    ) {
        super(name, origin, size, startTimeMs, durationMs, translation, id, 'RippleEffect');
        this.ripple_origin = ripple_origin;
        this.speed = speed;
    }

    static override emptyEffect() {
        return new RippleEffect(new Pair(0, 0), new Pair(20, 20), -1, -1, new Pair(0, 0), -1);
    }

    static fromEffect(effect: Effect, ripple_origin: Pair, speed: number) {
        // TODO: Splay the effect attributes for easy addition.
        return new RippleEffect(
            effect.origin,
            effect.size,
            effect.startTimeMs,
            effect.durationMs,
            ripple_origin,
            speed,
            effect.translation,
            effect.name,
            effect.id
        );
    }

    toJSON() {
        return {
            ...super.toJSON(),
            ripple_origin: this.ripple_origin,
            speed: this.speed,
        };
    }

    static fromJSON(data: any) {
        return RippleEffect.fromEffect(
            super.fromJSON(data),
            data.ripple_origin,
            data.speed
        );
    }
}
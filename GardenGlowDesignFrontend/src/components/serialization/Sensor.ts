import { duration } from "@mui/material";
import { Effect } from "./Effect";
import {Pair} from "./Pair";

export enum SensorType {
    BINARY = 1,
    ANALOG = 2,
}

export class Sensor {
    id: number;
    type: SensorType;
    location: Pair;
    pin: number | null;
    effects: Effect;
    duration: number;
    constructor(type: SensorType, location: Pair, id: number = -1, pin: number | null = null, effects: Effect, duration: number) {
        this.id = id;
        this.type = type;
        this.location = location;
        this.pin = pin;
        this.effects = effects;
        this.duration = duration;
    }

    toJSON() {
        return {
            id: this.id,
            type: this.type,
            location: this.location,
            pin: this.pin,
            effects: this.effects,
            duration: this.duration
        };
    }


    static fromJSON(data: any): Sensor {
        return new Sensor(
            data.type,
            Pair.fromJSON(data.location),
            data.id,
            data.pin,
            data.effects,
            data.duration
        );
    }


}
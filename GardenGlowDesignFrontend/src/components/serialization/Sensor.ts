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

    constructor(type: SensorType, location: Pair, id: number = -1, pin: number | null = null) {
        this.id = id;
        this.type = type;
        this.location = location;
        this.pin = pin;
    }
}
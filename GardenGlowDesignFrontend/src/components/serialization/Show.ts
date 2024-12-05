import {Effect} from "./Effect";
import {Layout} from "./Layout";
import {Sensor} from "./Sensor";

export class Show {
    name: string;
    durationMs: number;
    layouts: Layout[];
    effects: Effect[];
    sensors: Sensor[];

    constructor(name: string, duration: number) {
        this.name = name;
        this.durationMs = duration;
        this.effects = [];
        this.layouts = [];
        this.sensors = [];
    }

    private nextEffectId() {
        // Find the max id of all effects and return max + 1
        if (this.effects.length === 0) {
            return 1;
        }

        const max = () => this.effects.reduce((prev, effect) => {
            return effect.id > prev.id ? effect : prev;
        });

        return max().id + 1;
    }

    getFileName() : string {
        return this.name.replace(/ /g, '_');
    }

    getEffectById(id: number): Effect | undefined {
        return this.effects.find(effect => effect.id === id);
    }

    setEffects(effects: Effect[]) {
        this.effects = effects;
    }

    addEffect(effect: Effect) : number {
        if (effect.id === -1) {
            effect.id = this.nextEffectId();
        }
        this.effects.push(effect);
        return effect.id
    }

    addLayout(layout: Layout) {
        this.layouts.push(layout);
    }

    setLayouts(layouts: Layout[]) {
        this.layouts = layouts;
    }

    addSensor(sensor: Sensor) {
        this.sensors.push(sensor);
    }

    setSensors(sensors: Sensor[]) {
        this.sensors = sensors;
    }

    toJSON() {
        this.durationMs = this.effects[this.effects.length-1].durationMs + this.effects[this.effects.length-1].startTimeMs;
        return {
            name: this.name,
            duration: (this.durationMs) ?? 0,
            effects: this.effects.map(effect => effect.toJSON()),
            layouts: this.layouts.map(layout => layout.toJSON())
        };
    }

    exportToFile() {
        const dataStr = "data:text/json;charset=utf-8," +
            encodeURIComponent(JSON.stringify(this.toJSON()));
        const downloadAnchorNode = document.createElement('a');
        downloadAnchorNode.setAttribute("href", dataStr);
        downloadAnchorNode.setAttribute("download", this.getFileName() + '.json');
        document.body.appendChild(downloadAnchorNode);
        downloadAnchorNode.click();
        downloadAnchorNode.remove();
    }
}

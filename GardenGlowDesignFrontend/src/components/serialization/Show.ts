import { EffectType } from "../../types";
import {Effect, RippleEffect} from "./Effect";
import {GridLayout, Layout} from "./Layout";
import { Pair } from "./Pair";
import {Sensor} from "./Sensor";

export class Show {
    name: string;
    durationMs: number;
    layouts: Layout[];
    effects: Effect[];
    sensors: Sensor[];

    constructor(name: string) {
        this.name = name;
        this.durationMs = 0;
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
        if(effect.type === EffectType.ripple || effect.type === EffectType.ghostRipple || effect.type === EffectType.pumpkinGhostRipple){
            let o = new Pair(Math.floor((this.layouts[0] as GridLayout).width/2),Math.floor((this.layouts[0] as GridLayout).height/2));
            effect.origin= (o);
            if(effect.type === EffectType.ripple){
                (effect as RippleEffect).ripple_origin=o;
            }
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
        if(this.effects.length > 0){
            let dur = this.effects[this.effects.length-1].durationMs ?? 0;
            let start = this.effects[this.effects.length-1].startTimeMs ?? 0;
            this.durationMs = dur + start;     
        }
        else{
            this.durationMs = 0; 
        }
        return {
            name: this.name,
            duration: this.durationMs,
            effects: this.effects.map(effect => effect.toJSON()),
            layouts: this.layouts.map(layout => layout.toJSON())
        };
    }

    exportToFile() {
        const dataStr = "data:text/json;charset=utf-8," +
            encodeURIComponent(JSON.stringify(this.toJSON()));
        const downloadAnchorNode = document.createElement('a');
        downloadAnchorNode.setAttribute("href", dataStr);
        downloadAnchorNode.setAttribute("download", "show" + '.json');
        document.body.appendChild(downloadAnchorNode);
        downloadAnchorNode.click();
        downloadAnchorNode.remove();
    }
}

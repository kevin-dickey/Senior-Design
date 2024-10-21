import {Effect} from "./Effect";
import {Layout} from "./Layout";

export class Show {
    name: string;
    duration: number;
    layouts: Layout[];
    effects: Effect[];

    constructor(name: string, duration: number) {
        this.name = name;
        this.duration = duration;
        this.effects = [];
        this.layouts = [];
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

    toJSON() {
        return {
            name: this.name,
            duration: this.duration,
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

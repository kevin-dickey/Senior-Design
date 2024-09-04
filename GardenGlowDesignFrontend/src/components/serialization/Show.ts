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

    addEffect(effect: Effect) {
        this.effects.push(effect);
    }

    addLayout(layout: Layout) {
        this.layouts.push(layout);
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
        const fileName = `${this.name.replace(/ /g, '_')}.json`;
        downloadAnchorNode.setAttribute("href", dataStr);
        downloadAnchorNode.setAttribute("download", fileName);
        document.body.appendChild(downloadAnchorNode);
        downloadAnchorNode.click();
        downloadAnchorNode.remove();
    }
}

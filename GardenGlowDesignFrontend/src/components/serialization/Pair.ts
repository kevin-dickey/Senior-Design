export class Pair {
    x: number;
    y: number;

    constructor(x: number, y: number) {
        this.x = x;
        this.y = y;
    }

    toJSON() {
        return {
            x: this.x,
            y: this.y,
        };
    }

    static fromJSON(data: any): Pair {
        return new Pair(data.x, data.y);
    }
}
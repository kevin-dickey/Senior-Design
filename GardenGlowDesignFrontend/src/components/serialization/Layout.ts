export class Layout {

    toJSON() {
        return {};
    }
}

export class PixelGrid extends Layout {
    width: number;
    height: number;

    constructor(width: number, height: number) {
        super()
        this.width = Math.max(1, width);
        this.height = Math.max(1, height);
    }

    toJSON() {
        return {
            ...super.toJSON(),
            width: this.width,
            height: this.height
        };
    }
}
export class Layout {
    shape?: string | null = null;

    toJSON() {
        return {
            shape: this.shape
        };
    }
}

export class GridLayout extends Layout {
    shape = 'grid';
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
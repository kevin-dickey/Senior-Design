export class Layout {
    shape?: string | null = null;

    toJSON() {
        return {
            shape: this.shape
        };
    }

    static fromJSON(data: any) : Layout{
        const layout = new Layout();
        Object.assign(layout, data);
        return layout;
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
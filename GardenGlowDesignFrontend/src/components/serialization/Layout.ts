export class Layout {
    shape?: string | null = null;
    type: string = 'layout'; 

    toJSON() {
        return {
            shape: this.shape,
            type: this.type
        };
    }

    static fromJSON(data: any) : Layout{
        if (data.type === 'grid') {
            return GridLayout.fromJSON(data); 
        }
        data.width = data.width;
        data.height = data.height;
        const layout = new Layout();
        Object.assign(layout, data);
        return layout;
    }
}

export class GridLayout extends Layout {
    shape = 'grid';
    type = 'grid';
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
    
    static fromJSON(data: any): GridLayout {
        const layout = new GridLayout(data.width, data.height);
        data.width = data.width;
        data.height = data.height;
        Object.assign(layout, data);
        return layout;
    }

}
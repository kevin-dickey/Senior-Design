import {Show} from "../components/serialization/Show";
import {deserializeShow} from "../utils/deserializeShow";

interface IShowStorage {
    saveShow(path: string, show: Show): void;
    loadShow(path: string): Show;
    deleteShow(path: string): void;
    listShows(directory?: string): string[];
}

export class LocalStorageManager implements IShowStorage {
    private readonly rootKey = 'showFiles';

    saveShow(path: string, show: Show): void {
        const showData = JSON.stringify(show);
        localStorage.setItem(`${this.rootKey}/${path}`, showData);
    }

    loadShow(path: string): Show {
        const showData = localStorage.getItem(`${this.rootKey}/${path}`);
        if (!showData) {
            throw new Error(`Show file not found at path: ${path}`);
        }
        const parsedData = JSON.parse(showData);
        return deserializeShow(parsedData);
    }

    deleteShow(path: string): void {
        if (!localStorage.getItem(`${this.rootKey}/${path}`)) {
            throw new Error(`Show file not found at path: ${path}`);
        }
        localStorage.removeItem(`${this.rootKey}/${path}`);
    }

    listShows(directory: string = ''): string[] {
        const shows: string[] = [];
        const prefix = `${this.rootKey}/${directory}`;
        for (let i = 0; i < localStorage.length; i++) {
            const key = localStorage.key(i);
            if (key && key.startsWith(prefix)) {
                const relativePath = key.substring(this.rootKey.length + 1);
                if (directory && relativePath.indexOf('/') === -1) {
                    shows.push(relativePath);
                } else if (!directory) {
                    shows.push(relativePath);
                }
            }
        }
        return shows;
    }

    createFolder(folderName: string): void {
        localStorage.setItem(`${this.rootKey}/${folderName}/`, '');
    }
}

const storageManager = new LocalStorageManager();
export default storageManager;
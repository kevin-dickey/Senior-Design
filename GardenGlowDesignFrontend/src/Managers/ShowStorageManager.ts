import {Show} from "../components/serialization/Show";
import {deserializeShow} from "../utils/deserializeShow";

interface IShowStorage {
    saveShow(path: string, show: Show): void;
    loadShow(path: string): Promise<Show>;
    deleteShow(path: string): void;
    listShows(directory?: string): string[];
    listExampleShows(): Promise<string[]>;
}

export class LocalStorageManager implements IShowStorage {
    private readonly rootKey = 'showFiles';

    saveShow(path: string, show: Show): void {
        const showData = JSON.stringify(show);
        localStorage.setItem(`${this.rootKey}/${path}`, showData);
    }

    loadShow = async (path: string): Promise<Show> => {
        // If we have a show file starting with exampleShows/, it's not in local storage
        // and we should fetch it from the public folder.
        if (path.startsWith('/exampleShows/')) {
            const resp = await fetch(path);
            const showData = await resp.json();
            return deserializeShow(showData);
        }

        const showData = localStorage.getItem(`${this.rootKey}/${path}`);
        if (!showData) {
            throw new Error(`Show file not found at path: ${path}`);
        }
        const parsedData = JSON.parse(showData);
        return deserializeShow(parsedData);
    };

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

    listExampleShows = async (): Promise<string[]> => {
        // Read from the public/exampleShows/show-manifest.txt file and return the list of shows.
        const shows: string[] = [];
        // Fetch and read the text file
        const response = await fetch('/exampleShows/show-manifest.txt');
        const text = await response.text();
        // Split the text into lines
        const lines = text.split('\n');
        // Remove any empty lines
        const validLines = lines.filter(line => line.length > 0);
        // Add each line to the shows array
        validLines.forEach(line => shows.push(line));
        return shows;
    };

    createFolder(folderName: string): void {
        localStorage.setItem(`${this.rootKey}/${folderName}/`, '');
    }
}

const storageManager = new LocalStorageManager();
export default storageManager;
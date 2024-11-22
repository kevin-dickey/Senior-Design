import {Show} from "../components/serialization/Show";
import {deserializeShow} from "../utils/deserializeShow";
import {Folder} from "../components/pages/FoldersOverview";

interface IShowStorage {
    saveShow(path: string, show: Show): void;

    getShowInfo(path: string): Promise<{ name: string, path: string }>;

    loadShow(path: string): Promise<Show>;

    deleteShow(path: string): void;

    listShows(directory?: string): Promise<Folder[]>;

    listExampleShows(): Promise<Folder[]>;
}

export class LocalStorageManager implements IShowStorage {
    private readonly rootKey = 'showFiles';

    saveShow(path: string, show: Show): void {
        const showData = JSON.stringify(show);
        localStorage.setItem(`${this.rootKey}/${path}`, showData);
    }

    async getAndParseJSON(path: string): Promise<any> {
        // If we have a show file starting with exampleShows/, it's not in local storage
        // and we should fetch it from the public folder.
        if (path.startsWith('exampleShows/')) {
            const resp = await fetch('/' + path);
            return await resp.json();
        } else {
            const showData = localStorage.getItem(`${this.rootKey}/${path}`);
            if (!showData) {
                throw new Error(`Show file not found at path: ${path}`);
            }
            return JSON.parse(showData);
        }
    }

    async getShowInfo(path: string): Promise<{ name: string, path: string }> {
        try {
            const showJSON = await this.getAndParseJSON(path)

            if (!showJSON.name || !showJSON.effects) {
                console.error('Show file is missing name or effects');
            }
            return {
                name: showJSON.name,
                path: showJSON.path
            }
        } catch (e) {
            throw new Error(`Show file not found at path: ${path}. ${e}`);
        }
    }

    loadShow = async (path: string): Promise<Show> => {
        const parsedData = await this.getAndParseJSON(path);
        return deserializeShow(parsedData);
    };

    deleteShow(path: string): void {
        if (!localStorage.getItem(`${this.rootKey}/${path}`)) {
            throw new Error(`Show file not found at path: ${path}`);
        }
        localStorage.removeItem(`${this.rootKey}/${path}`);
    }

    listShows(directory: string = ''): Promise<Folder[]> {
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
        return this.reduceFiles(shows);
    }

    // TODO: This and some other non-explicitly LOCAL storage related items
    //  can be moved upward
    reduceFiles = async (files: string[]): Promise<Folder[]> => {
        const folderMap: { [key: string]: Folder } = {};

        for (const file of files) {
            const parts = file.split('/');
            const folderName = parts[0];

            if (!folderMap[folderName]) {
                folderMap[folderName] = {name: folderName, files: []};
            }

            if (!file.endsWith('/')) {
                try {
                    const showInfo = await storageManager.getShowInfo(file)
                    console.log(`Loaded show: ${showInfo.name}`);
                    folderMap[folderName].files.push({name: showInfo.name, path: file});
                } catch (e) {
                    console.error(e);
                }
            }
        }

        return Object.values(folderMap);
    };

    listExampleShows = async (): Promise<Folder[]> => {
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
        return this.reduceFiles(shows);
    };

    listAllShows = async (): Promise<Folder[]> => {
        let loadedFolders: Folder[] = [];
        loadedFolders = loadedFolders.concat(await storageManager.listShows());
        loadedFolders = loadedFolders.concat(await storageManager.listExampleShows());
        return loadedFolders;
    }

    createFolder(folderName: string): void {
        localStorage.setItem(`${this.rootKey}/${folderName}/`, '');
    }
}

const storageManager = new LocalStorageManager();
export default storageManager;
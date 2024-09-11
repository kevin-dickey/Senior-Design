import React, {useState} from 'react';
import {Link} from "react-router-dom";
import CssBaseline from '@mui/material/CssBaseline';
import Grid from '@mui/material/Grid';
import Typography from '@mui/material/Typography';
import Container from '@mui/material/Container';
import {createTheme, ThemeProvider} from '@mui/material/styles';
import {List, ListItem, Paper} from '@mui/material';

import storageManager from "../../Managers/ShowStorageManager";

const darkTheme = createTheme({
    palette: {
        mode: 'dark',
    },
});


interface File {
    name: string;
    path: string;
}

interface Folder {
    name: string;
    files: File[];
}

interface OverviewProps {
    folders: Folder[];
}

const reduceFiles = (files: string[]) : Folder[] => {
    return files.reduce((acc: Folder[], file: string) => {
        const parts = file.split('/');
        const folderName = parts[0];

        // Find the folder in the accumulator
        let folder = acc.find(f => f.name === folderName);
        if (!folder) {
            folder = {name: folderName, files: []};
            acc.push(folder);
        }

        folder.files.push({
            name: storageManager.loadShow(file).name,
            path: file,
        });
        return acc;
    }, []);
}

const Overview: React.FC<OverviewProps> = () => {
    const [folders, setFolders] = useState(reduceFiles(storageManager.listShows()));

    return (
        <ThemeProvider theme={darkTheme}>
            <Container component="main">
                <CssBaseline/>

                <Grid container spacing={2}>
                    {folders.map((folder, index) => (
                        <Grid item xs={12} sm={6} md={4} lg={3} key={index}>
                            <Paper elevation={3} style={{padding: '16px'}}>
                                <Typography variant="h6">{folder.name}</Typography>
                                <List>
                                    {folder.files.map((file, fileIndex) => (
                                        <ListItem key={fileIndex}>
                                            <Link to='/configuration' state={{ path: file.path }}>
                                                <Typography variant="body2">{file.name}</Typography>
                                            </Link>
                                        </ListItem>
                                    ))}
                                </List>
                            </Paper>
                        </Grid>
                    ))}
                    <Grid item xs={12}>
                        <Link to='/configuration'>
                            <Typography variant="h6">Create New Show</Typography>
                        </Link>
                    </Grid>
                </Grid>
            </Container>
        </ThemeProvider>
    );
};

export default Overview;



import React from 'react';
import CssBaseline from '@mui/material/CssBaseline';
import Grid from '@mui/material/Grid';
import Typography from '@mui/material/Typography';
import Container from '@mui/material/Container';
import {createTheme, ThemeProvider} from '@mui/material/styles';
import {List, ListItem, Paper} from '@mui/material';

import {ShowFileExport} from "../serialization/ShowFileExport";
import {Show} from "../serialization/Show";
import {RainbowEffect} from "../serialization/Effect";
import {GridLayout} from "../serialization/Layout";
import { CreateRainbowEffectFormContainer } from "../editors/RainbowEffectForm/CreateRainbowEffectFormContainer";

const makeShow = () => {
    const show = new Show('Basic Show File', 10000);
    const effect = RainbowEffect.emptyEffect();
    show.addEffect(effect);
    const grid = new GridLayout(10, 10);
    show.addLayout(grid);
    return show;
}

const show = makeShow();

const darkTheme = createTheme({
    palette: {
        mode: 'dark',
    },
});


interface File {
    name: string;
}

interface Folder {
    name: string;
    files: File[];
}

interface OverviewProps {
    folders: Folder[];
}

const Overview: React.FC<OverviewProps> = ({folders}) => {
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
                                            <Typography variant="body2">{file.name}</Typography>
                                        </ListItem>
                                    ))}
                                </List>
                            </Paper>
                        </Grid>
                    ))}
                </Grid>
            </Container>

            <ShowFileExport show={show}/>
            <CreateRainbowEffectFormContainer onSubmit={(effect: any) => console.log(effect)} />
        </ThemeProvider>
    );
};

export default Overview;



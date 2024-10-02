import React, {useState} from 'react';
import {
    Grid,
    Paper,
    Typography,
    Container,
    CssBaseline,
    List,
    ListItem,
    Button,
    Box
} from '@mui/material';
import {ThemeProvider} from '@mui/material/styles';
import FilesOverview from './FilesOverview';
import {useNavigate} from 'react-router-dom';
import {Show} from "../serialization/Show";
import darkTheme from "../../utils/Theming";


export interface File {
    name: string;
    path: string;
}

export interface Folder {
    name: string;
    files: File[];
}

export interface FoldersOverviewProps {
    folders: Folder[] | null;
    // Totally an anti-pattern to have these here, but I don't feel like refactoring.
    loading: boolean;
    onAddFolder: (folderName: string) => void;
    onAddFile: (fileName: string, folderName: string, width: number, height: number) => void;
    loadShow: (path: string) => Promise<Show>;
}

const FoldersOverview: React.FC<FoldersOverviewProps> = ({
                                                             folders,
                                                             loading,
                                                             onAddFolder,
                                                             onAddFile,
                                                             loadShow
                                                         }) => {
    const [selectedFolder, setSelectedFolder] = useState<Folder | null>(null);
    const navigate = useNavigate();

    const handleFolderClick = (folder: Folder) => {
        setSelectedFolder(folder);
    };

    const handleBackClick = () => {
        setSelectedFolder(null);
    };

    const handleFileClick = (file: File) => {
        if (file.path.startsWith('/exampleShows/')) {
            console.log("Creating a new show from example: " + file.name);
            loadShow(file.path).then(show => {
                navigate('/configuration', {state: {show: show}});
            });
        }
        navigate('/configuration', {state: {path: file.path}});
    }

    if (loading) {
        return (
            <Box>
                <Typography variant="h4">Loading...</Typography>
            </Box>
        )
    }

    return (
        <ThemeProvider theme={darkTheme}>
            <Container component="main">
                <CssBaseline/>
                {selectedFolder ? (
                    <FilesOverview
                        folderName={selectedFolder.name}
                        files={selectedFolder.files}
                        onBack={handleBackClick}
                    />
                ) : (
                    <>
                        <Button
                            onClick={() => navigate('/shows/add-folder')}
                            variant="contained"
                            color="primary">
                            New Folder
                        </Button>
                        <Button
                            onClick={() => navigate('/shows/add-file')} variant="contained"
                            color="secondary" sx={{ml: 2}}>
                            New File
                        </Button>
                        <Grid container spacing={2}>
                            {folders && folders.length > 0 && (
                                folders.map((folder, index) => (
                                    <Grid item xs={12} sm={6} md={4} lg={3} key={index}>
                                        <Paper elevation={3} style={{padding: '16px'}}>
                                            <Typography
                                                variant="h6"
                                                style={{cursor: 'pointer'}}
                                                onClick={() => handleFolderClick(folder)}
                                            >
                                                {folder.name}
                                            </Typography>
                                            <List>
                                                {folder.files.map((file, fileIndex) => (
                                                    <ListItem
                                                        key={fileIndex}
                                                        onClick={() => handleFileClick(file)}
                                                        style={{cursor: 'pointer'}}
                                                    >
                                                        <Typography
                                                            variant="body2">
                                                            {file.name}
                                                        </Typography>
                                                    </ListItem>
                                                ))}
                                            </List>
                                        </Paper>
                                    </Grid>
                                ))
                            )}
                        </Grid>
                    </>
                )}
            </Container>
        </ThemeProvider>
    );
};

export default FoldersOverview;

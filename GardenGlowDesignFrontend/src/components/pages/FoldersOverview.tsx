import React, { useState } from 'react';
import { Grid, Paper, Typography, Container, CssBaseline, Button, List, ListItem } from '@mui/material';
import { createTheme, ThemeProvider } from '@mui/material/styles';
import FilesOverview from './FilesOverview';
import { useNavigate } from 'react-router-dom';

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

interface FoldersOverviewProps {
  folders: Folder[];
  onAddFolder: (folderName: string) => void;
  onAddFile: (fileName: string, folderName: string) => void;
}

const FoldersOverview: React.FC<FoldersOverviewProps> = ({ folders, onAddFolder, onAddFile }) => {
  const [selectedFolder, setSelectedFolder] = useState<Folder | null>(null);
  const navigate = useNavigate();

  const handleFolderClick = (folder: Folder) => {
    setSelectedFolder(folder);
  };

  const handleBackClick = () => {
    setSelectedFolder(null);
  };

  const handleFileClick = (fileName: string) => {
    console.log(fileName); 
    navigate('/configuration', { state: { fileName } });  
  };

  return (
    <ThemeProvider theme={darkTheme}>
      <Container component="main">
        <CssBaseline />
        {selectedFolder ? (
          <FilesOverview
            folderName={selectedFolder.name}
            files={selectedFolder.files}
            onBack={handleBackClick}
          />
        ) : (
          <>
            <Button onClick={() => navigate('/add-folder')} variant="contained" color="primary">
              New Folder
            </Button>
            <Button onClick={() => navigate('/add-file')} variant="contained" color="secondary" sx={{ ml: 2 }}>
              New File
            </Button>
            <Grid container spacing={2} sx={{ mt: 2 }}>
              {folders.map((folder, index) => (
                <Grid item xs={12} sm={6} md={4} lg={3} key={index}>
                  <Paper elevation={3} style={{ padding: '16px' }}>
                    <Typography
                      variant="h6"
                      style={{ cursor: 'pointer' }}
                      onClick={() => handleFolderClick(folder)}
                    >
                      {folder.name}
                    </Typography>
                    <List>
                      {folder.files.map((file, fileIndex) => (
                        <ListItem
                          key={fileIndex}
                          onClick={() => handleFileClick(file.name)}
                          style={{ cursor: 'pointer' }}
                        >
                          <Typography variant="body2">{file.name}</Typography>
                        </ListItem>
                      ))}
                    </List>
                  </Paper>
                </Grid>
              ))}
            </Grid>
          </>
        )}
      </Container>
    </ThemeProvider>
  );
};

export default FoldersOverview;

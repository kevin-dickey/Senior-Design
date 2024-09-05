import React, { useState } from 'react';
import { Grid, Paper, Typography, Container, CssBaseline, List, ListItem } from '@mui/material';
import { createTheme, ThemeProvider } from '@mui/material/styles';
import FilesOverview from './FilesOverview';

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
}

const FoldersOverview: React.FC<FoldersOverviewProps> = ({ folders }) => {
  const [selectedFolder, setSelectedFolder] = useState<Folder | null>(null);

  const handleFolderClick = (folder: Folder) => {
    setSelectedFolder(folder);
  };

  const handleBackClick = () => {
    setSelectedFolder(null);
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
          <Grid container spacing={2}>
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
                      <ListItem key={fileIndex}>
                        <Typography variant="body2">{file.name}</Typography>
                      </ListItem>
                    ))}
                  </List>
                </Paper>
              </Grid>
            ))}
          </Grid>
        )}
      </Container>
    </ThemeProvider>
  );
};

export default FoldersOverview;

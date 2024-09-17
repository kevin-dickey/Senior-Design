import React from 'react';
import { List, ListItem, Typography, Paper } from '@mui/material';
import { useNavigate } from 'react-router-dom';  

interface File {
  name: string;
}

interface FilesOverviewProps {
  folderName: string;
  files: File[];
  onBack: () => void;
}

const FilesOverview: React.FC<FilesOverviewProps> = ({ folderName, files, onBack }) => {
  const navigate = useNavigate(); 

  const handleFileClick = (fileName: string) => {
    console.log(fileName);
    navigate('/configuration', { state: { fileName } }); 
  };
  
  return (
    <Paper elevation={3} style={{ padding: '16px' }}>
      <Typography variant="body1" onClick={onBack} style={{ cursor: 'pointer', paddingBottom:'2vh'}}>
        ← Back to Folders
      </Typography>
      <Typography variant="h5">{folderName}</Typography>
      <List>
        {files.map((file, index) => (
          <ListItem key={index} onClick={() => handleFileClick(file.name)} style={{ cursor: 'pointer' }}>
            <Typography variant="body2">{file.name}</Typography>
          </ListItem>
        ))}
      </List>
    </Paper>
  );
};

export default FilesOverview;

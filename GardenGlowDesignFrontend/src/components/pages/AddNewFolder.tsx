import React, { useState } from 'react';
import { Button, TextField, Box } from '@mui/material';
import { useNavigate } from 'react-router-dom';

interface AddNewFolderProps {
  onAddFolder: (folderName: string) => void;
}

const AddNewFolder: React.FC<AddNewFolderProps> = ({ onAddFolder }) => {
  const [folderName, setFolderName] = useState('');
  const navigate = useNavigate();

  const handleSave = () => {
    if (folderName) {
      onAddFolder(folderName);
      navigate('/foldersoverview');
    }
  };

  return (
    <Box sx={{ p: 2 }}>
      <TextField
        label="Folder Name"
        value={folderName}
        onChange={(e) => setFolderName(e.target.value)}
        fullWidth
      />
      <Box sx={{ display: 'flex', gap: 2, mt: 2 }}>
        <Button variant="contained" color="primary" onClick={handleSave}>
          Save
        </Button>
        <Button variant="outlined" color="secondary" onClick={() => navigate('/foldersoverview')}>
          Cancel
        </Button>
      </Box>
    </Box>
  );
};

export default AddNewFolder;

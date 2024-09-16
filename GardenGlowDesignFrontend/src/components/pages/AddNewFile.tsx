import React, {useState} from 'react';
import {Button, TextField, Box, MenuItem, Select, FormControl, InputLabel} from '@mui/material';
import {useNavigate} from 'react-router-dom';
import {Folder} from "./FoldersOverview";

interface AddNewFileProps {
    folders: Folder[];
    onAddFile: (fileName: string, folderName: string) => void;
}

const AddNewFile: React.FC<AddNewFileProps> = ({folders, onAddFile}) => {
    const [fileName, setFileName] = useState('');
    const [selectedFolder, setSelectedFolder] = useState('');
    const navigate = useNavigate();

    const handleSave = () => {
        if (fileName && selectedFolder) {
            onAddFile(fileName, selectedFolder);
            navigate('/shows');
        }
    };

    return (
        <Box sx={{p: 2}}>
            <TextField
                label="File Name"
                value={fileName}
                onChange={(e) => setFileName(e.target.value)}
                fullWidth
            />
            <FormControl fullWidth sx={{mt: 2}}>
                <InputLabel>Folder</InputLabel>
                <Select
                    value={selectedFolder}
                    onChange={(e) => setSelectedFolder(e.target.value)}
                >
                    {folders.map((folder) => (
                        <MenuItem key={folder.name} value={folder.name}>
                            {folder.name}
                        </MenuItem>
                    ))}
                </Select>
            </FormControl>
            <Box sx={{display: 'flex', gap: 2, mt: 2}}>
                <Button variant="contained" color="primary" onClick={handleSave}>
                    Save
                </Button>
                <Button
                    variant="outlined"
                    color="secondary"
                    onClick={() =>
                        navigate('/shows')
                    }
                >
                    Cancel
                </Button>
            </Box>
        </Box>
    );
};

export default AddNewFile;

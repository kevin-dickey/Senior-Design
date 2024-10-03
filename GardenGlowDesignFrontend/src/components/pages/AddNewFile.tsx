import React, {useState} from 'react';
import {
    Button,
    TextField,
    Box,
    MenuItem,
    Select,
    FormControl,
    InputLabel,
} from '@mui/material';
import {ThemeProvider} from '@mui/material/styles';
import {useNavigate} from 'react-router-dom';
import {Folder} from "./FoldersOverview";
import darkTheme from "../../utils/Theming";
import CssBaseline from "@mui/material/CssBaseline";

interface AddNewFileProps {
    folders: Folder[];
    onAddFile: (fileName: string, folderName: string, width: number, height: number) => void;
}

const AddNewFile: React.FC<AddNewFileProps> = ({folders, onAddFile}) => {
    const [fileName, setFileName] = useState('');
    const [width, setWidth] = useState<number>();
    const [height, setHeight] = useState<number>();
    const [selectedFolder, setSelectedFolder] = useState('');
    const navigate = useNavigate();

    const handleSave = () => {
        if (fileName && selectedFolder && width && height) {
            onAddFile(fileName, selectedFolder, width, height);
            navigate('/shows');
        }
    };

    return (
        <ThemeProvider theme={darkTheme}>
            <CssBaseline />
            <Box sx={{p: 2}}>
                <TextField
                    required
                    label="File Name"
                    value={fileName}
                    onChange={(e) => setFileName(e.target.value)}
                    fullWidth
                />
                <FormControl fullWidth sx={{mt: 2}}>
                    <InputLabel>Folder</InputLabel>
                    <Select
                        required
                        value={selectedFolder}
                        onChange={(e) => setSelectedFolder(e.target.value)}
                        variant='outlined'
                    >
                        {folders.map((folder) => (
                            <MenuItem key={folder.name} value={folder.name}>
                                {folder.name}
                            </MenuItem>
                        ))}
                    </Select>
                </FormControl>
                <TextField
                    sx={{mt: 2}}
                    required
                    label="Lights Width"
                    type="number"
                    value={width}
                    onChange={(e) => setWidth(+e.target.value)}
                    fullWidth
                />
                <TextField
                    sx={{mt: 2}}
                    required
                    label="Lights Height"
                    type="number"
                    value={height}
                    onChange={(e) => setHeight(+e.target.value)}
                    fullWidth
                />
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
        </ThemeProvider>
    )
        ;
};

export default AddNewFile;

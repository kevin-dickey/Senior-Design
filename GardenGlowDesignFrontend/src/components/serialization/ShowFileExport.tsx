import React from 'react';

import { Show } from './Show';

import {Button} from "@mui/material";
import {FileDownload} from '@mui/icons-material';
interface ShowFileExportProps {
  show: Show;
}

export const ShowFileExport: React.FC<ShowFileExportProps> = ({ show }) => {
  const handleExport = () => {
    show.exportToFile();
  };

  return (
    <Button
        variant="contained"
        startIcon={<FileDownload />}
        onClick={handleExport}
    >
      Export Show File
    </Button>
  );
};

export default ShowFileExport;
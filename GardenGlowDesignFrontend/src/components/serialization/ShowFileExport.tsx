import React from 'react';
import { Show } from './Show';

interface ShowFileExportProps {
  show: Show;
}

export const ShowFileExport: React.FC<ShowFileExportProps> = ({ show }) => {
  const handleExport = () => {
    show.exportToFile();
  };

  return (
    <button onClick={handleExport}>
      Export Show File
    </button>
  );
};

export default ShowFileExport;
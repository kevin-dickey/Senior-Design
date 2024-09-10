// OverviewContainer.tsx
import React from 'react';
import Overview from '../components/pages/Overview';

const fakeData = [
  {
    name: 'Folder 1',
    files: [
      { name: 'File 1', path: 'folder1/file1' },
      { name: 'File 2', path: 'folder1/file2' },
    ],
  },
  {
    name: 'Folder 2',
    files: [
      { name: 'File 3', path: 'folder2/file3' },
      { name: 'File 4', path: 'folder2/file4' },
    ],
  },
  {
    name: 'Folder 3',
    files: [
      { name: 'File 5', path: 'folder3/file5' },
      { name: 'File 6', path: 'folder3/file6' },
    ],
  },
];

const OverviewContainer: React.FC = () => {
  return <Overview folders={fakeData} />;
};

export default OverviewContainer;

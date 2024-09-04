// OverviewContainer.tsx
import React from 'react';
import Overview from '../components/pages/Overview';

const fakeData = [
  {
    name: 'Folder 1',
    files: [
      { name: 'File 1' },
      { name: 'File 2' },
    ],
  },
  {
    name: 'Folder 2',
    files: [
      { name: 'File 3' },
      { name: 'File 4' },
    ],
  },
  {
    name: 'Folder 3',
    files: [
      { name: 'File 5' },
      { name: 'File 6' },
    ],
  },
];

const OverviewContainer: React.FC = () => {
  return <Overview folders={fakeData} />;
};

export default OverviewContainer;

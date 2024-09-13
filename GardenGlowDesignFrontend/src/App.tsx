import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import HomePage from './components/pages/Home';
import Configuration from './components/pages/Configuration';
import './App.css';
import LoginContainer from './containers/LoginContainer';
import OverviewContainer from './containers/OverviewContainer';
import AddNewFolder from './components/pages/AddNewFolder';
import AddNewFile from './components/pages/AddNewFile';
import LEDGrid from './components/pages/Grid';

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="/login" element={<LoginContainer />} />
        <Route path="/configuration" element={<Configuration />}/>
        <Route path="/foldersoverview" element={<OverviewContainer  />}/>
        <Route path="/add-folder" element={<AddNewFolder onAddFolder={() => {}} />} />
        <Route path="/add-file" element={<AddNewFile onAddFile={() => { } } folders={[]} />} />
        <Route path="/grid" element={<LEDGrid/>} />


      </Routes>
    </Router>
  );
}

export default App;

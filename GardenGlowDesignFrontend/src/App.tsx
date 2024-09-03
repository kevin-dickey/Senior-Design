import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import HomePage from './components/pages/Home';
import ConfigurationPage from './components/pages/Configuration';
import './App.css';
import LoginContainer from './containers/LoginContainer';
import OverviewContainer from './containers/OverviewContainer';

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="/login" element={<LoginContainer />} />
        <Route path="/configuration" element={<ConfigurationPage />}/>
        <Route path="/overview" element={<OverviewContainer  />}/>
      </Routes>
    </Router>
  );
}

export default App;

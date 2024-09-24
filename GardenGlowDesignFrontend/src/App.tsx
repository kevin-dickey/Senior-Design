import React from 'react';
import {BrowserRouter as Router, Routes, Route} from 'react-router-dom';
import HomePage from './components/pages/Home';
import Configuration from './components/pages/Configuration';
import './App.css';
import LoginContainer from './containers/LoginContainer';
import OverviewContainer from './containers/OverviewContainer';

function App() {
    return (
        <Router>
            {/*TODO: A <Routes> block bitches when nothing matches it.
                Because we have another in OverviewContainer, we get errors.*/}
            <Routes>
                <Route path="/" element={<HomePage/>}/>
                <Route path="/login" element={<LoginContainer/>}/>
                <Route path="/configuration" element={<Configuration/>}/>
            </Routes>
            <OverviewContainer/>
        </Router>
    );
}

export default App;

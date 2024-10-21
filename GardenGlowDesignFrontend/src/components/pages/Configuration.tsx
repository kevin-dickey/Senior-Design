// Main configuration page for the application.
// This page is where the user can create, edit, and delete effects, as well as
// saving and loading shows!
import React, {ChangeEvent, useEffect, useState} from 'react';
import {useLocation, useNavigate} from "react-router-dom"

import {Box} from '@mui/material';
import {ThemeProvider} from '@mui/material/styles';
import CssBaseline from "@mui/material/CssBaseline";

import NavBar from "../NavBar";
import {Show} from "../serialization/Show";
import {Effect} from "../serialization/Effect";
import {makeShow} from "../../Managers/ConfigurationManager";
import storageManager from "../../Managers/ShowStorageManager";
import {CreateEffectFormContainer} from "../editors/CreateEffectFormContainer";
import {EditEffectFormContainer} from "../editors/EditEffectFormContainer";
import {validateEffects} from "../editors/EffectList";
import {EntityPalette} from "../../containers/EntityPalette";
import GridContainer from '../../containers/GridContainer';
import {TimelineContainer} from "../../containers/TimelineContainer";

import darkTheme from "../../utils/Theming";
import "./Configuration.css";
import {DialogContainer} from "../../containers/DialogContainer";
import {GridLayout} from "../serialization/Layout";


const Configuration: React.FC = () => {
    const location = useLocation();
    const navigate = useNavigate();

    const [loadingShow, setLoadingShow] = useState(true);
    const [showPath, setShowPath] = useState<string | null>(null);
    const [show, setShow] = useState<Show | null>(null);
    const [selectedEffectId, setSelectedEffectId] = useState<number | null>(null);
    const [creatingEffectType, setCreatingEffectType] = useState<string>('');
    const [creatingNewEffect, setCreatingNewEffect] = useState(false);
    const [showConfigPanelOpen, setShowConfigPanelOpen] = useState(false);

    useEffect(() => {
        if (location.state) {
            if (location.state.show) {
                console.log("Loading show:" + location.state.show);
                setShow(location.state.show);

                if (location.state.path) {
                    setShowPath(location.state.path);
                } else {
                    console.warn('Path not set in location state. Must prompt user for save location.');
                }
            } else if (location.state.path) {
                console.log("Loading show from path: " + location.state.path);
                storageManager.loadShow(location.state.path).then(loadedShow => setShow(loadedShow));
                setShowPath(location.state.path);
            }
        } else {
            console.warn('No location state found. Creating a new show.');
            setShow(makeShow());
        }
        setLoadingShow(false);
    }, [location.state]);

    const updateEffect = (submittedEffect: Effect, effectToUpdateId: number) => {
        if (show == null) {
            throw Error("Show must not be null!");
        }
        const updatedShow = new Show(show.name, show.duration);
        updatedShow.setLayouts(show.layouts);
        updatedShow.setEffects(show.effects.map(effect => {
            if (effect.id === effectToUpdateId) {
                return submittedEffect;
            }
            return effect;
        }));
        setShow(updatedShow);
    };

    const deleteEffect = (effectId: number) => {
        if (show == null) {
            throw Error("Show must not be null!");
        }

        const updatedShow = new Show(show.name, show.duration);
        updatedShow.setEffects(show.effects.filter(effect => effect.id !== effectId));
        setShow(updatedShow);
    }

    // TODO: Save As
    const saveShow = async (show: Show) => {
        console.log('Saving show: ' + show.name);
        console.log(show);

        const errors = await validateEffects(show.effects);
        if (errors.length > 0) {
            console.log(`Errors found: ${errors.map((e: any) => e.message).join(', ')}`);
            console.log('Show not saved. Please fix errors and try again.');
            return;
        }
        // FIXME: For now this is fine, but once we open configuration without a file, we need to
        //  prompt the user for a folder & file name to save under.
        if (showPath == null) {
            console.error('No show path set. Prompt the user for a path!');
            return;
        }
        storageManager.saveShow(showPath, show);
        console.log('Show saved successfully');
    }

    function startEffect(): void {
        throw new Error('Function not implemented.');
    }

    // noinspection JSUnusedLocalSymbols
    function handleInputChange(event: ChangeEvent<HTMLInputElement>): void {
        throw new Error('Function not implemented.');
    }

    // noinspection JSUnusedLocalSymbols
    function handleEffectChange(event: ChangeEvent<HTMLSelectElement>): void {
        throw new Error('Function not implemented.');
    }

    return (
        <ThemeProvider theme={darkTheme}>
            <CssBaseline/>
            {loadingShow && <div>Loading...</div>}
            {!loadingShow && show && (
                <Box sx={{display: 'flex', flexDirection: 'column', height: '100vh'}}>
                    <NavBar
                        showName={show.name}
                        onClickSettings={() => {
                            console.log("Settings clicked")
                            setShowConfigPanelOpen(true)
                        }}
                        onClickAccount={() => console.log("Account clicked")}
                        onClickHome={() => navigate('/shows')}
                        onClickSave={() => saveShow(show)}
                        onClickExport={() => {
                            console.log("Export Show")
                            show.exportToFile();
                        }}
                    />
                    <Box
                        sx={{
                            display: 'flex',
                            flexGrow: 1,
                            bgcolor: '#181818',
                            color: '#ffffff',
                            overflow: 'hidden'
                        }}
                    >
                        {/* Sidebar */}
                        <EntityPalette
                            show={show}
                            saveShow={saveShow}
                            selectedEffectId={selectedEffectId}
                            setSelectedEffectId={setSelectedEffectId}
                            createEffectType={creatingEffectType}
                            setCreateEffectType={setCreatingEffectType}
                            creatingNewEffect={creatingNewEffect}
                            setCreatingNewEffect={setCreatingNewEffect}
                            startEffect={startEffect}
                            handleInputChange={handleInputChange}
                            handleEffectChange={handleEffectChange}
                        />

                        <Box
                            sx={{
                                width: '100%',
                                height: '100%',
                                display: 'flex',
                                flexDirection: 'column',
                                overflow: 'hidden',
                            }}
                        >
                            {/* Grid Container */}
                            <Box
                                id="grid-container"
                                sx={{
                                    flexGrow: 1,
                                    overflow: 'hidden',
                                    display: 'flex',
                                    flexDirection: 'column',
                                }}
                            >
                                {creatingNewEffect &&
                                    <Box sx={{bgcolor: '#3a3a3a', p: 2}}>
                                        <CreateEffectFormContainer
                                            effectType={creatingEffectType}
                                            onSubmit={(values) => {
                                                show.addEffect(values);
                                                setCreatingNewEffect(false);
                                            }}
                                        />
                                    </Box>
                                }
                                {selectedEffectId != null &&
                                    <Box sx={{bgcolor: '#3a3a3a', p: 2}}>
                                        <EditEffectFormContainer
                                            key={selectedEffectId}
                                            // TODO: This will error if selectedEffectId isn't present in .effects
                                            effect={show.getEffectById(selectedEffectId)!}
                                            onSubmit={(effect: any) => {
                                                console.log("Saving effect: " + effect);
                                                updateEffect(effect, selectedEffectId);
                                            }}
                                            onDelete={(effectId: number) => {
                                                console.log("Deleting effect: " + effectId);
                                                deleteEffect(effectId);
                                                setSelectedEffectId(null);
                                            }}
                                        />
                                    </Box>
                                }

                                <GridContainer show={show}/>
                            </Box>

                            {/* Timeline Container */}
                            <Box
                                bgcolor="#2a2a2a"
                                paddingTop={1}
                                zIndex={1}
                            >
                                {/**add better time indicator */}
                                <TimelineContainer
                                    effects={show.effects}
                                    onChangeEffects={(effects: Effect[]) => console.log(effects)}
                                    onChangeSelectedEffectId={(effectId: number) => {
                                        effectId === selectedEffectId ?
                                            setSelectedEffectId(null) :
                                            setSelectedEffectId(effectId);
                                    }}
                                />
                            </Box>
                        </Box>
                    </Box>
                </Box>
            )}

            {/* Show Settings Pane */}
            <DialogContainer
                show={show}
                open={showConfigPanelOpen}
                onSubmit={(formValues) => {
                    console.log("Saving show settings");
                    console.log(formValues);
                    const newShow = new Show(formValues.showName, formValues.durationSeconds * 1000);
                    newShow.addLayout(new GridLayout(formValues.width, formValues.height));

                    if (show) {
                        for (const effect of show.effects) {
                            newShow.addEffect(effect);
                        }
                    }

                    setShow(newShow);
                    setShowConfigPanelOpen(false);
                }}
                onClose={() => setShowConfigPanelOpen(false)}
            />
        </ThemeProvider>
    );
};

export default Configuration;
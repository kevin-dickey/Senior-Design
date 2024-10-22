// Main configuration page for the application.
// This page is where the user can create, edit, and delete effects, as well as
// saving and loading shows!
import React, {ChangeEvent, useState} from 'react';
import {useNavigate} from "react-router-dom"

import {Box} from '@mui/material';
import {ThemeProvider} from '@mui/material/styles';
import CssBaseline from "@mui/material/CssBaseline";

import NavBar from "../NavBar";
import {Show} from "../serialization/Show";
import {Effect} from "../serialization/Effect";
import {CreateEffectFormContainer} from "../editors/CreateEffectFormContainer";
import {EditEffectFormContainer} from "../editors/EditEffectFormContainer";
import {validateEffects} from "../editors/EffectList";
import {EntityPalette} from "../../containers/EntityPalette";
import {TimelineContainer} from "../../containers/TimelineContainer";

import darkTheme from "../../utils/Theming";
import "./Configuration.css";
import {GridLayout} from "../serialization/Layout";
import {ConfigPanel} from "../../containers/ConfigPanel";
import {DialogContainer} from "../../containers/DialogContainer";
import GridContainer from "../../containers/GridContainer";
import AddNewFile from "./AddNewFile";
import {Folder} from "./FoldersOverview";
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";


export interface ConfigurationProps {
    show: Show | null;
    setShow: (show: Show) => void;
    onSaveShow: (savePath?: string, show?: Show) => void;

    // Deals with saving the show as a new file
    savingShowAs: boolean;
    setSavingShowAs: (saving: boolean) => void;
    folders: Folder[];
}

const Configuration: React.FC<ConfigurationProps> = (
    {
        show, setShow, onSaveShow,
        savingShowAs, setSavingShowAs, folders,
    }) => {
    const navigate = useNavigate();

    const [selectedEffectId, setSelectedEffectId] = useState<number | null>(null);
    const [creatingEffectType, setCreatingEffectType] = useState<string>('');
    const [creatingNewEffect, setCreatingNewEffect] = useState(false);
    const [showConfigPanelOpen, setShowConfigPanelOpen] = useState(false);

    const updateEffect = (submittedEffect: Effect, effectToUpdateId: number) => {
        if (show == null) {
            throw Error("Show must not be null!");
        }
        const updatedShow = new Show(show.name, show.durationMs);
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

        const updatedShow = new Show(show.name, show.durationMs);
        updatedShow.setEffects(show.effects.filter(effect => effect.id !== effectId));
        setShow(updatedShow);
    }

    const updateShowSettings = (show: Show,
                                name: string,
                                durationSeconds: number,
                                gridHeight: number,
                                gridWidth: number) => {
        console.log("Saving show settings");
        const newShow = new Show(name, durationSeconds * 1000);
        newShow.addLayout(new GridLayout(gridWidth, gridHeight));

        if (show) {
            for (const effect of show.effects) {
                newShow.addEffect(effect);
            }
        }

        setShow(newShow);
        setShowConfigPanelOpen(false);
    }

    const handleSaveShow = async (show: Show) => {
        console.log('Saving show: ' + show.name);
        console.log(show);

        const errors = await validateEffects(show.effects);
        if (errors.length > 0) {
            console.log(`Errors found: ${errors.map((e: any) => e.message).join(', ')}`);
            console.log('Show not saved. Please fix errors and try again.');
            return;
        }

        onSaveShow();
    }

    const handleSubmitSaveAsPrompt = async (fileName: string, folderName: string, width: number, height: number) => {
        const sanitized_name = fileName.replace(/ /g, '_');
        const path = folderName + '/' + sanitized_name;
        console.log('Adding new file: ' + path);
        console.log('Width: ' + width);
        console.log('Height: ' + height);

        const newShow = new Show(fileName, show?.durationMs || 0);
        newShow.addLayout(new GridLayout(width, height));
        newShow.setEffects(show?.effects || []);
        newShow.setSensors(show?.sensors || []);

        onSaveShow(path, newShow);

        // Clean up
        setSavingShowAs(false);
    }

    const closeEffectPane = () => {
        // TODO: Add a confirmation dialog if the user has unsaved changes
        setSelectedEffectId(null);
        setCreatingNewEffect(false);
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

    if (creatingNewEffect && selectedEffectId != null) {
        throw new Error('Cannot create a new effect and edit an existing effect at the same time.');
    }

    return (
        <ThemeProvider theme={darkTheme}>
            <CssBaseline/>
            {show && (
                <>
                    {folders != null && (
                        <Dialog
                            open={savingShowAs}
                            onClose={() => setSavingShowAs(false)}
                        >
                            <DialogTitle>Show: Save As</DialogTitle>
                            <DialogContent>
                                <AddNewFile
                                    folders={folders}
                                    onAddFile={handleSubmitSaveAsPrompt}
                                />
                            </DialogContent>
                        </Dialog>
                    )}
                    <Box sx={{display: 'flex', flexDirection: 'column', height: '100vh'}}>
                        <NavBar
                            showName={show.name}
                            onClickSettings={() => {
                                console.log("Settings clicked")
                                setShowConfigPanelOpen(true)
                            }}
                            onClickAccount={() => console.log("Account clicked")}
                            onClickHome={() => navigate('/shows')}
                            onClickSave={() => handleSaveShow(show)}
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
                                    {/* Selected Effect Modal */}
                                    <ConfigPanel
                                        title={(creatingNewEffect ? "Create" : "Edit") + " Effect"}
                                        open={selectedEffectId != null || creatingNewEffect}
                                        handleClose={closeEffectPane}
                                    >
                                        {selectedEffectId != null && (
                                            <EditEffectFormContainer
                                                key={selectedEffectId}
                                                // TODO: This will error if selectedEffectId isn't present in .effects
                                                effect={show.getEffectById(selectedEffectId!)!}
                                                onSubmit={(effect: any) => {
                                                    console.log("Saving effect: " + effect);
                                                    updateEffect(effect, selectedEffectId!);
                                                    setSelectedEffectId(null);
                                                }}
                                                onDelete={(effectId: number) => {
                                                    console.log("Deleting effect: " + effectId);
                                                    deleteEffect(effectId);
                                                    setSelectedEffectId(null);
                                                }}
                                            />
                                        )}
                                        {creatingNewEffect && (
                                            <CreateEffectFormContainer
                                                effectType={creatingEffectType}
                                                onSubmit={(values) => {
                                                    console.log("Creating new effect: " + values);
                                                    // TODO: Handle if values cannot be cast
                                                    //  to Effect
                                                    show.addEffect(values);
                                                    setCreatingNewEffect(false);
                                                }}
                                            />
                                        )}
                                    </ConfigPanel>
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
                    <DialogContainer
                        show={show}
                        open={showConfigPanelOpen}
                        onSubmit={(formValues) => {
                            updateShowSettings(show,
                                formValues.showName,
                                formValues.durationSeconds,
                                formValues.height,
                                formValues.width);
                        }}
                        onClose={() => setShowConfigPanelOpen(false)}
                    />

                </>
            )}
        </ThemeProvider>
    );
};

export default Configuration;
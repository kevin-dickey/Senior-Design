import React, {useState, useEffect, ChangeEvent} from 'react';
import {useLocation} from "react-router-dom"
import {Box} from '@mui/material';
import {ThemeProvider} from '@mui/material/styles';
import CssBaseline from "@mui/material/CssBaseline";
import {validateEffects} from "../editors/EffectList";
import {Show} from "../serialization/Show";
import {Effect, RainbowEffect} from "../serialization/Effect";
import {GridLayout} from "../serialization/Layout";
import storageManager from "../../Managers/ShowStorageManager";
import {Pair} from "../serialization/Pair";
import {CreateEffectFormContainer} from "../editors/CreateEffectFormContainer";
import {EditEffectFormContainer} from "../editors/EditEffectFormContainer";
import {EntityPalette} from "../../containers/EntityPalette";
import {TimelineContainer} from "../../containers/TimelineContainer";
import NavBar from "../NavBar";
import darkTheme from "../../utils/Theming";
import GridContainer from '../../containers/GridContainer';

import "./Configuration.css";


const makeShow = () => {
    const show = new Show('Basic Show File', 10000);
    const effect = RainbowEffect.emptyEffect();

    show.addEffect(effect);
    const effect2 = new RainbowEffect(new Pair(0, 0), new Pair(16, 16),
        1000, 1000, ['#420', '#696969'], 1000, 'Crazy Train');
    show.addEffect(effect2);

    const grid = new GridLayout(10, 10);
    show.addLayout(grid);

    return show;
}

const Configuration: React.FC = () => {
    const location = useLocation();

    const [loadingShow, setLoadingShow] = useState(true);
    const [show, setShow] = useState<Show | null>(null);
    const [selectedEffectId, setSelectedEffectId] = useState<number | null>(null);
    const [creatingEffectType, setCreatingEffectType] = useState<string>('');
    const [creatingNewEffect, setCreatingNewEffect] = useState(false);

    useEffect(() => {
        if (location.state && location.state.path) {
            console.log('Loaded show!: ' + location.state.path);
            const serializedShow = storageManager.loadShow(location.state.path);
            console.log(serializedShow);
            setShow(serializedShow);
        } else {
            console.log('Creating new show!');
            setShow(makeShow());
        }
        setLoadingShow(false);
    }, [location.state]);

    const updateEffect = (submittedEffect: Effect, effectToUpdateId: number) => {
        if (show == null) {
            throw Error("Show must not be null!");
        }
        const updatedShow = new Show(show.name, show.duration);
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
        storageManager.saveShow(location.state.path, show);
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
                        onClickSettings={() => console.log("Settings clicked")}
                        onClickAccount={() => console.log("Account clicked")}
                        onClickHome={() => console.log("Home clicked")}
                        onClickSave={() => saveShow(show)}
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
                                p={2}
                                zIndex={1}
                            >
                                {/**add better time indicator */}
                                <TimelineContainer
                                    effects={show.effects}
                                    onChangeEffects={(effects: Effect[]) => console.log(effects)}
                                />
                            </Box>
                        </Box>
                    </Box>
                </Box>
            )}
        </ThemeProvider>
    );
};

export default Configuration;
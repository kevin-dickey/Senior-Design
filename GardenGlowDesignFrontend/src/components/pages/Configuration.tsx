import React, {useState} from 'react';
import {Box} from '@mui/material';
import {TransformWrapper, TransformComponent} from "react-zoom-pan-pinch";
import {validateEffects} from "../editors/EffectList";
import {Show} from "../serialization/Show";
import {Effect, RainbowEffect} from "../serialization/Effect";
import {GridLayout} from "../serialization/Layout";
import {Pair} from "../serialization/Pair";
import {CreateEffectFormContainer} from "../editors/CreateEffectFormContainer";
import {EditEffectFormContainer} from "../editors/EditEffectFormContainer";
import {EntityPalette} from "../../containers/EntityPalette";
import {TimelineContainer} from "../../containers/TimelineContainer";

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
    const [show, setShow] = useState<Show | null>(makeShow());
    const [selectedEffectId, setSelectedEffectId] = useState<number | null>(null);
    const [creatingEffectType, setCreatingEffectType] = useState<string>('');
    const [creatingNewEffect, setCreatingNewEffect] = useState(false);

    const updateEffect = (submittedEffect: Effect, effectToUpdateId: number) => {
        // Create a copy of the show
        // Update the effect in the copy
        // Set the show to the copy
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

    const saveShow = async (show: Show) => {
        console.log('Saving show: ' + show.name);
        console.log(show);

        const errors = await validateEffects(show.effects);
        if (errors.length > 0) {
            console.log(`Errors found: ${errors.map((e: any) => e.message).join(', ')}`);
            console.log('Show not saved. Please fix errors and try again.');
            return;
        }
        console.log('Show not saved... Not yet implemented!');
    }

    return (
        <div>
            {show &&
                <Box
                    sx={{
                        display: 'flex',
                        height: '100vh',
                        bgcolor: '#181818',
                        color: '#ffffff',
                        overflow: 'hidden'
                    }}>
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
                    />

                    <Box
                        sx={{
                            width: '100%',
                            height: '100%',
                            position: 'absolute',
                            top: 0,
                            left: 0,
                            overflow: 'hidden',
                        }}
                    >
                        {/* Grid Container */}
                        <Box
                            sx={{
                                width: '85%',
                                height: '85%',
                                position: 'absolute',
                                top: 0,
                                right: 0,
                                overflow: 'hidden',
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

                            <TransformWrapper
                                initialScale={1}
                                wheel={{step: 0.5}}
                                minScale={.5}
                                maxScale={5}
                            >
                                {({zoomIn, zoomOut, resetTransform}) => (
                                    <TransformComponent wrapperStyle={{flex: 1}}>
                                        <Box flexDirection="column">
                                            {/* Generate a grid of dots to represent LEDs */}
                                            {[...Array(50)].map((_, rowIndex) => (
                                                <Box key={rowIndex} display="flex" gap={0.5}>
                                                    {[...Array(50)].map((_, colIndex) => (
                                                        <Box
                                                            key={colIndex}
                                                            sx={{
                                                                width: 10,
                                                                height: 10,
                                                                bgcolor: '#222',
                                                                borderRadius: '50%',
                                                            }}
                                                        />
                                                    ))}
                                                </Box>
                                            ))}
                                        </Box>
                                    </TransformComponent>
                                )}
                            </TransformWrapper>
                        </Box>

                        {/* Timeline Container */}
                        <Box
                            position="absolute"
                            bottom={0}
                            right={0}
                            width="80%"
                            height="60%"
                            bgcolor="#2a2a2a"
                            p={2}
                            zIndex={1}
                        >
                            {/**add better time indicator */}
                            <TimelineContainer/>
                        </Box>
                    </Box>
                </Box>
            }
        </div>
    );
}


export default Configuration;
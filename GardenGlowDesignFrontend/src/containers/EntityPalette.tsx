import {
    Box,
    Button,
    Divider,
    Drawer,
    FormControl,
    InputLabel,
    MenuItem,
    Select
} from "@mui/material";
import { ExpandLess, ExpandMore } from "@mui/icons-material";
import { EffectList } from "../components/editors/EffectList";
import React, { useState } from "react";
import { Show } from "../components/serialization/Show";
import Toolbar from "@mui/material/Toolbar";
import { EffectType } from "../types";

export interface EntityPaletteProps {
    show: Show;
    selectedEffectId: number | null;
    setSelectedEffectId: (id: number | null) => void;
    createEffectType: EffectType;
    setCreateEffectType: (effectType: EffectType) => void;
    creatingNewEffect: boolean;
    setCreatingNewEffect: (creatingNewEffect: boolean) => void;
    handleInputChange: (event: React.ChangeEvent<HTMLInputElement>) => void;
    handleEffectChange: (event: React.ChangeEvent<HTMLSelectElement>) => void;
}

// TODO: Make this responsive or sum
const drawerWidth = 240;

export const EntityPalette: React.FC<EntityPaletteProps> = (props) => {
    const [isShapesOpen, setIsShapesOpen] = useState(true);
    const [isEffectsOpen, setIsEffectsOpen] = useState(true);
    const [isColorsOpen, setIsColorsOpen] = useState(true);
    const [isEffectsListOpen, setIsEffectsListOpen] = useState(true);

    return (
        <Drawer
            variant="permanent"
            anchor="left"
            sx={{
                width: drawerWidth,
                flexShrink: 0,
                [`& .MuiDrawer-paper`]: { width: drawerWidth, boxSizing: 'border-box' },
            }}
        >
            <Toolbar />
            <Box sx={{ overflow: 'auto' }}>
                <Box>
                    <Button
                        fullWidth onClick={() => setIsShapesOpen(!isShapesOpen)}
                        sx={{ color: '#fff', justifyContent: 'flex-start' }}>
                        Shapes {isShapesOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isShapesOpen &&
                        <Box sx={{ bgcolor: '#3a3a3a', p: 2 }}>Shapes content</Box>}
                </Box>
                <Divider sx={{ bgcolor: '#444' }} />
                <Box>
                    <Button
                        fullWidth
                        onClick={() => setIsEffectsOpen(!isEffectsOpen)}
                        sx={{ color: '#fff', justifyContent: 'flex-start' }}
                    >
                        Effects {isEffectsOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isEffectsOpen &&
                        <Box sx={{ bgcolor: '#3a3a3a', p: 2 }}>Effects content</Box>}
                </Box>
                <Divider sx={{ bgcolor: '#444' }} />
                <Box>
                    <Button
                        fullWidth
                        onClick={() => setIsColorsOpen(!isColorsOpen)}
                        sx={{ color: '#fff', justifyContent: 'flex-start' }}>
                        Colors {isColorsOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isColorsOpen && (
                        <Box sx={{ bgcolor: '#3a3a3a', p: 2 }}>
                            <input type="color" value="#9731f2" readOnly={true} />
                        </Box>
                    )}
                </Box>
                <Divider sx={{ bgcolor: '#444' }} />
                <Box>
                    <Button
                        fullWidth
                        onClick={() => setIsEffectsListOpen(!isEffectsListOpen)}
                        sx={{ color: '#fff', justifyContent: 'flex-start' }}
                    >
                        Effects in Show {isEffectsListOpen ? <ExpandLess /> : <ExpandMore />}
                    </Button>
                    {isEffectsListOpen && (
                        <div>
                            <EffectList
                                effects={props.show.effects}
                                onEffectSelected={(effectId: number) => {
                                    const finalSelectedId = props.selectedEffectId === effectId ? null : effectId;
                                    props.setSelectedEffectId(finalSelectedId);
                                }} />
                            <FormControl fullWidth>
                                <InputLabel id="effect-type-label">Effect Type</InputLabel>
                                <Select
                                    labelId="effect-type-label"
                                    id="effect-type"
                                    value={props.createEffectType}
                                    label="Effect Type"
                                    onChange={(e) => props.setCreateEffectType(e.target.value as EffectType)}
                                    variant='outlined'
                                >
                                    <MenuItem value="rainbow">Rainbow</MenuItem>
                                    <MenuItem value="ripple">Ripple</MenuItem>
                                    <MenuItem value="pumpkinRainbow">Pumpkin Rainbow</MenuItem>
                                    <MenuItem value="pumpkinRipple">Pumpkin Ripple</MenuItem>
                                    <MenuItem value="ghostRainbow">Ghost Rainbow</MenuItem>
                                    <MenuItem value="ghostRipple">Ghost Ripple</MenuItem>
                                    <MenuItem value="pumpkinGhostRainbow">Pumpkin Ghost Rainbow</MenuItem>
                                    <MenuItem value="pumpkinGhostRipple">Pumpkin Ghost Ripple</MenuItem>
                                    <MenuItem value="snowflake">Snowflake</MenuItem>
                                    <MenuItem value="snowman">Snowman</MenuItem>
                                    <MenuItem value="christmasTree">Christmas Tree</MenuItem>
                                    <MenuItem value="candyCane">Candy Cane</MenuItem>
                                </Select>
                            </FormControl>
                            <Button
                                onClick={() => {
                                    if (!props.createEffectType) {
                                        // TODO: Display a warning that the effect type must
                                        //   be selected. Maybe use formik for this
                                        return;
                                    }
                                    props.setSelectedEffectId(null);
                                    props.setCreatingNewEffect(true);
                                }}
                            >
                                Add Effect
                            </Button>
                        </div>
                    )}
                </Box>
            </Box>
        </Drawer>
    );
}
// Draggable and closable component wrapper with a title.
import * as React from 'react';
import Draggable from 'react-draggable';

import Dialog from '@mui/material/Dialog';
import DialogActions from "@mui/material/DialogActions";
import DialogContent from '@mui/material/DialogContent';
import DialogTitle from '@mui/material/DialogTitle';
import Paper, {PaperProps} from '@mui/material/Paper';

import CloseRoundedIcon from '@mui/icons-material/CloseRounded';
import {IconButton} from "@mui/material";

function PaperComponent(props: PaperProps) {
    return (
        <Draggable
            handle="#draggable-dialog-title"
            cancel={'[class*="MuiDialogContent-root"]'}
        >
            <Paper {...props} />
        </Draggable>
    );
}

export interface ConfigPanelProps {
    title: string;
    open: boolean;
    handleClose: () => void;
    actions?: React.ReactNode;
    children: React.ReactNode;
}

export function ConfigPanel(props: ConfigPanelProps) {
    return (
        <Dialog
            open={props.open}
            onClose={props.handleClose}
            PaperComponent={PaperComponent}
            slotProps={{
                // Override the default MUI dialog background shading for transparency.
                backdrop: {
                    sx: {
                        backgroundColor: 'rgba(0, 0, 0, 0)',
                    },
                },
            }}
            aria-labelledby="draggable-dialog-title"
        >
            <DialogTitle
                id="draggable-dialog-title"
                style={{
                    cursor: 'move',
                    display: 'flex',
                    justifyContent: 'space-between',
                    alignItems: 'center',
                }}>
                {props.title}
                <IconButton
                    onClick={props.handleClose}
                >
                    <CloseRoundedIcon/>
                </IconButton>
            </DialogTitle>
            <DialogContent>
                {props.children}
            </DialogContent>
            {props.actions && (
                <DialogActions>
                    {props.actions}
                </DialogActions>
            )}
        </Dialog>
    );
}

export declare const console: _PrimitiveConsole;
export declare const globalThis: object;

/* Used by Duktape::enc(2) */
type Replacer = ((key: string, value: any) => undefined | null | any) 
                | string[];
type Reviver = (key: string, value: any) => undefined | any;

export class _PrimitiveConsole
{
    /**
     * Creates a primitive console. 
     * Will always throw a ReferenceError upon trying to instantiate it. 
     */
    constructor();

    /**
     * Prints *content* to console.
     * @param content The content to be printed.
     */
    Log(content: string): void;
}

/**
 * The result of a `Duktape.act()` call.
 */
export interface CallStackFrameInfo
{
    /** 
     * The current function being executed. 
     */
    function: Function
    pc: number
    lineNumber: number
}

export function GetCurrentLine(): number;

export class Duktape
{
    static version: number;
    static env: string;

    static fin(obj: any): any;
    static fin(obj: object, func?: (finalizer: object) => void): undefined;

    static enc(encoding: "hex" | "base64", content: string): string;
    static enc(encoding: "jx" | "jc", content: object, replacer?: Replacer, space?: number): string;
    static dec(encoding: "hex" | "base64", content: string): Object;
    static dec(encoding: "jx" | "jc", content: string, reviver?: Reviver): object;

    static act(level: number): CallStackFrameInfo;
    
    static gc(flags: number): void;

    static compact(x: object): object;

    static errCreate(e: Error): Error;
    static errThrow(e: Error): Error;
}

export class CBOR
{
    static encode(item: object | any[]): string;
    static decode(item: string): object | any[];
}

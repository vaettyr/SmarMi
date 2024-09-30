export default function App() {
    
    const connect = async () => {
        const device = await navigator.bluetooth.requestDevice();
        const server = await device.gatt?.connect();
        const service = await server?.getPrimaryService("derp");
    }

    return (
        <div>

        </div>
    )
}